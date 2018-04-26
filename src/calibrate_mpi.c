#include <assert.h>
#include "debug.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_EML
#include <eml.h>
#endif

#include <mpi.h>
#include "calibrate_mpi.h"

#define DBG_FMT "%8g"

typedef enum direction {
    LEFT = -1,
    NONE = 0,
    RIGHT = 1
} Direction;

struct cal_energy_strategy {
    // Calibration interval for measuring energy
    double measurement_interval;
    double first_energy_calibration_time;
    size_t iteration_interval;
    int interval_calc_started;
    int calibrating;
    size_t calibrate_until;

    double* values;
    double* work_per_joule;
    double* copy_work_per_joule;

    // Energy heuristic approach parameters
    double ratio_step; // How much do you move
    Direction last_direction; // Where did you move
    int inverted; // Have you tried the other possible movements
    double previous_total_sum; // How much did the execution cost
    // Restart based on execution progress
    int total_it;
    int last_reset;
    // Restart based on a probability
    int restart_step; // restart_value is incremented restart_step every time it fails
    int restart_value; // simulated annealing style parameter
};

struct cal_calibration {
    MPI_Comm comm; // @suppress("Type cannot be resolved")
    int comm_size;
    int comm_rank;
    int comm_root;
    enum cal_strategy strategy;
    enum cal_strategy current_strategy;

    double threshold;
    size_t total_count;
    size_t current_iteration;

    double* timevalues;

    // Calibration
    int started;
    int* counts;
    int* displs;
    int work_blocksize;

    // Strategies
    struct cal_energy_strategy* energy;
};

const char* EML_DEV_TO_USE = "nvml0";
const int INTERVAL_ITERATIONS = 10; // Number of iterations to estimate a measurement interval
// It is innacurate for irregular problems
const int CAL_TAG_RECALIBRATING = 1;
const int CAL_TAG_CALIBRATED = 2;
const int CAL_MAX_STEPS = 3;

static Direction _invert_direction(Direction d) {
    if (d == LEFT)
        return RIGHT;
    else
        return LEFT;
}

static enum cal_strategy _select_strategy(struct cal_calibration* calib) {
    //XXX if calib energy is currently active, check it has ended to change it
    if (calib->current_strategy & CAL_STRATEGY_ENERGY) {
        if (calib->energy->calibrating)
            return CAL_STRATEGY_ENERGY;
    }
    return calib->strategy;
}

// Determines if the movement is in range
static int _is_movement_legal(double current_ratio, double ratio_step,
        Direction dir) {
    if (current_ratio + ratio_step * dir > 0)
        return 0;
    return 1;
}

static void _invert_ratios(double* ratios, double* current_ratios,
        double ratio_step, int last_direction) {
    ratios[0] = current_ratios[0] - ratio_step * last_direction * 4; // * 2 to undo calib->ratio_step /= 2
    ratios[1] = current_ratios[1] + ratio_step * last_direction * 4; // * 2 to invert the values
} // XXX Generalize

// Performs the movement of the heuristic, if not possible returns a terrible energy value to avoid the movement
static int _calculate_move(struct cal_calibration* calib, double* ratios,
        double* current_ratios, double* total_energy_sum, Direction direction) {
    double simulated_energy = 0;
    Direction *dirs = malloc(calib->comm_size * sizeof(Direction));
    for (int i = 0; i < calib->comm_size; i++) {
        if (!_is_movement_legal(current_ratios[i], calib->energy->ratio_step,
                direction)) {
            simulated_energy += (current_ratios[i]
                    + calib->energy->ratio_step * direction)
                    * calib->total_count / calib->energy->work_per_joule[i];
            dirs[i] = direction;
        } else {
            simulated_energy = 99999999;
            dirs[i] = NONE;
        }
        direction = _invert_direction(direction);
    }
    direction = dirs[0];

    dbglog_info(" Simulated Energy %d: %.2f\n", direction, simulated_energy);
    for (int i = 0; i < calib->comm_size; i++) {
        dbglog_info("                     %.2f * %zu / %.2f\n", current_ratios[i] + calib->energy->ratio_step * dirs[i],
                calib->total_count, calib->energy->work_per_joule[i]);
    }

    if (simulated_energy < *total_energy_sum) {
        dbglog_info("              MOVING RATIOS:");
        for (int i = 0; i < calib->comm_size; i++) {
            ratios[i] = current_ratios[i] + calib->energy->ratio_step * dirs[i];
            dbglog_append(" %.2f -> %.2f", current_ratios[i], ratios[i]);
        } dbglog_append("\n");
        *total_energy_sum = simulated_energy;
        calib->energy->last_direction = dirs[0];
        return 1;
    }
    free(dirs);
    return 0;
}

static int calibrate_root(struct cal_calibration* calib) {
    double tmax = DBL_MIN;
    double tmin = DBL_MAX;
    double taccum = 0;
    // Calculates accumulated time values max and min
    dbglog_info(" -- Current strategy: (%d) ", calib->current_strategy);
    if (calib->current_strategy == CAL_STRATEGY_NONE)   dbglog_append("CAL_STRATEGY_NONE ");
    if (calib->current_strategy & CAL_STRATEGY_TIME)   dbglog_append("CAL_STRATEGY_TIME ");
    if (calib->current_strategy & CAL_STRATEGY_ENERGY) dbglog_append("CAL_STRATEGY_ENERGY ");
    dbglog_append("\n");
    dbglog_info("     Calib Iteration: %zu\n", calib->current_iteration);

    dbglog_info("      current counts: ");
    for (int i = 0; i < calib->comm_size; i++) {
        dbglog_append("%d ", calib->counts[i]);
    } 
    dbglog_append("\n");

    if (calib->current_strategy == CAL_STRATEGY_NONE) {
        return CAL_TAG_RECALIBRATING;
    }

    dbglog_info("          time spent: ");
    for (int i = 0; i < calib->comm_size; i++) {
        dbglog_append("%.4f ", calib->timevalues[i]);
        taccum += calib->timevalues[i];
        if (calib->timevalues[i] == 0)
            continue;
        if (calib->timevalues[i] > tmax)
            tmax = calib->timevalues[i];
        if (calib->timevalues[i] < tmin)
            tmin = calib->timevalues[i];
    } dbglog_append("\n");

    // Calculates accumulated energy values max and min
    if (calib->current_strategy & CAL_STRATEGY_ENERGY) {
        // Obtiene la eficiencia de cada proceso (energia / operaciones)
        dbglog_info("        energy spent: ");
        for (int i = 0; i < calib->comm_size; i++) {
//      dbglog_info("calib->energyvalues[%d] = %.2f\n", i, calib->energyvalues[i]);
            dbglog_append("%.4f ", calib->energy->values[i]);
        } dbglog_append("\n");

        dbglog_info("    energy-per-count: ");
        for (int i = 0; i < calib->comm_size; i++) {
            dbglog_append("%8g " , calib->energy->values[i] / calib->counts[i]);
        } dbglog_append("\n");

        if (calib->energy->ratio_step < calib->threshold
                && calib->energy->inverted > 0) {
            // XXX RESTART
            int r = rand() % 1000; // Restart value int between 0 and 1000
            if (calib->energy->restart_value > r) {
//      if (calib->current_it / calib->total_it > calib->last_reset) {
//        calib->last_reset = calib->current_it / calib->total_it;
                calib->energy->ratio_step = 0.25;
                calib->energy->inverted = 0;
                calib->energy->last_direction = NONE;
                calib->energy->previous_total_sum = DBL_MAX;
                dbglog_info("    Not Calibrating but Restarting.\n");
            } else {
                //    calib->restart_value += calib->restart_step;
                dbglog_info("    Not Calibrating nor Restarting.\n");
            }
            return CAL_TAG_CALIBRATED;
        }
    } else {
        // Con solo tiempo comprueba la calibracion y el threshold
        if ((tmax == DBL_MIN) || (tmin == DBL_MAX)
                || ((1 - tmin / tmax) < calib->threshold))
            return CAL_TAG_CALIBRATED;
    }

    //dbglog_info("    raw time values: ");
//  for (int i = 0; i < calib->comm_size; i++) {
//  dbglog_append(DBG_FMT " ", calib->timevalues[i]);
//  }
//  dbglog_append("\n");

    dbglog_info("         calibrating \n");
    // Calcula el ratio de trabajo (tiempo por unidad de trabajo) de los nodos
    dbglog_info("        time ratios: ");
    double time_ratios[calib->comm_size];
    double time_ratio_sum = 0;
    for (int i = 0; i < calib->comm_size; i++) {
        if (calib->timevalues[i] == 0)
            time_ratios[i] = 0;
        else
            time_ratios[i] = calib->counts[i] / calib->timevalues[i];

        dbglog_append(DBG_FMT " ", time_ratios[i]);
        time_ratio_sum += time_ratios[i];
    } dbglog_append("\n");

    dbglog_info("             counts: ");
    for (int i = 0; i < calib->comm_size; i++) {
        dbglog_append("%d ", calib->counts[i]);
    } dbglog_append("(%zu) ", calib->total_count); dbglog_append("\n");

    if (calib->current_strategy & CAL_STRATEGY_ENERGY) {
        dbglog_info("           e-values: ");
        for (int i = 0; i < calib->comm_size; i++) {
            dbglog_append(DBG_FMT " ", calib->energy->values[i]);
        } dbglog_append("\n");

        dbglog_info("              watts: ");
        double watts;
        for (int i = 0; i < calib->comm_size; i++) {
            watts = calib->energy->values[i] / calib->timevalues[i];
            dbglog_append(DBG_FMT " ", watts);
        } dbglog_append("\n");
    }

    double work_per_joule_sum = 0.0;
    double aux_work_per_joule = 0.0;
    double total_energy_sum = 0.0;
    double current_ratios[calib->comm_size];
    if (calib->current_strategy & CAL_STRATEGY_ENERGY) {
        dbglog_info("     work-per-joule: ");
        for (int i = 0; i < calib->comm_size; i++) {
            current_ratios[i] = (double) calib->counts[i]
                    / (double) calib->total_count;

            if (calib->energy->values[i] <= 0 || calib->timevalues[i] <= 0) {
                //dbglog_append("\nEnergy or time equal to 0 ????\n");
                //dbglog_append("  %.2f J,  %.2f s\n\n", calib->energyvalues[i], calib->timevalues[i]);
                aux_work_per_joule = 0.0;
                return CAL_TAG_RECALIBRATING;
            } else {
                total_energy_sum += calib->energy->values[i];
//        aux_work_per_joule = calib->energyvalues[i] / calib->timevalues[i];
//        aux_work_per_joule /= calib->counts[i];
//        aux_work_per_joule = 1 / aux_work_per_joule;
                aux_work_per_joule = calib->counts[i]
                        / calib->energy->values[i];
            }
//      if (calib->work_per_joule[i] < aux_work_per_joule) {
//        dbglog_append(DBG_FMT "(p) ");
            calib->energy->work_per_joule[i] = aux_work_per_joule;
//      }
            dbglog_append(DBG_FMT " ", calib->energy->work_per_joule[i]);
            work_per_joule_sum += calib->energy->work_per_joule[i];
        } dbglog_append("\n");
    }

    // Reduce los ratios a un valor entre 0 y 1, y se tiene en cuenta porcentualmente la energia
    // y el tiempo

    double ratios[calib->comm_size];
    double ratio_sum = 0;

    if (calib->current_strategy & CAL_STRATEGY_ENERGY) {
        int stop_recalibration = 0;
        int current_steps = 0;
        double comparison = total_energy_sum / calib->energy->previous_total_sum
                - 1;
        for (int i = 0; i < calib->comm_size; i++) {
            ratios[i] = current_ratios[i]; // Required for the cases when no move is performed
        } dbglog_info("      Starting step: %.2f\n", calib->energy->ratio_step); dbglog_info("       Total Energy: %.2f (prev: %.2f, comparison: %.2f)\n", total_energy_sum, calib->energy->previous_total_sum, comparison);

        if (comparison < calib->threshold || calib->energy->inverted == 2) { // If previos movement is good, comparison should be less than threshold
            calib->energy->inverted = 0;
            while (!stop_recalibration && current_steps < CAL_MAX_STEPS) {
                // stop_recalibration in {0, 1} If movement is valid, stop_recalibration == 1
                stop_recalibration = _calculate_move(calib, ratios,
                        current_ratios, &total_energy_sum, LEFT);
                stop_recalibration |= _calculate_move(calib, ratios,
                        current_ratios, &total_energy_sum, RIGHT);

                current_steps += 1;
                calib->energy->ratio_step /= 2;

                if (!stop_recalibration) {
                    dbglog_info("              NOT MOVING, new step = %.2f\n", calib->energy->ratio_step);
                }
            } // While
        } else {
            // inverting the operation in the following sum (compared to the if above) inverts the operation
            dbglog_info("              INVERTING, %.3f, %.3f, %.3f, %d\n", current_ratios[0], current_ratios[1], calib->energy->ratio_step, calib->energy->last_direction);
            _invert_ratios(ratios, current_ratios, calib->energy->ratio_step,
                    calib->energy->last_direction);
            calib->energy->last_direction = _invert_direction(
                    calib->energy->last_direction);
            calib->energy->inverted += 1;
            dbglog_info("                 VALUES, %.2f -> %.2f, %.2f -> %.2f\n",
                    current_ratios[0], ratios[0], current_ratios[1], ratios[1]);
        }

        calib->energy->previous_total_sum = total_energy_sum;

        for (int i = 0; i < calib->comm_size; i++) {
            ratio_sum += ratios[i];
        }

    }
    if (calib->current_strategy & CAL_STRATEGY_TIME) {
        for (int i = 0; i < calib->comm_size; i++) {
            ratios[i] = time_ratios[i] / time_ratio_sum;
            ratio_sum += ratios[i];
        }
    }

    // Rounding
    int excess = 0;
    int ordered_ratios_index[calib->comm_size];
    for (int i = 0; i < calib->comm_size; i++) {
        ratios[i] = ratios[i] / ratio_sum;
        ratios[i] = floor(ratios[i] * 100);
        excess += ratios[i];
        ordered_ratios_index[i] = i;
    }
    excess = 100 - excess;

    // Obtain ordered indexes for applying excess (Insertion sort)
    for (int i = 1; i < calib->comm_size; i++) {
        int j = i;
        int aux;
        while (j > 0
                && ratios[ordered_ratios_index[j]]
                        < ratios[ordered_ratios_index[j - 1]]) {
            aux = ordered_ratios_index[j];
            ordered_ratios_index[j] = ordered_ratios_index[j - 1];
            ordered_ratios_index[j - 1] = aux;
            j--;
        }
    }

    // Largest remainder method for ensuring sum == 1

    double dbg_ratio_sum = 0;
    for (int i = 0; i < calib->comm_size; i++) {
        if (excess > 0) {
            ratios[ordered_ratios_index[i]] = (ratios[ordered_ratios_index[i]]
                    + 1) / 100;
            excess -= 1;
        } else {
            ratios[ordered_ratios_index[i]] = ratios[ordered_ratios_index[i]]
                    / 100;
        }
    }

    dbglog_info("  normalized ratios: ");
    for (int i = 0; i < calib->comm_size; i++) {
        dbglog_append( DBG_FMT " ", ratios[i] );
        dbg_ratio_sum += ratios[i];
    } dbglog_append("\n");

    dbglog_info("dbg_ratio_sum = %.2f (should be 1)\n", dbg_ratio_sum);
    // Asigna las unidades de trabajo a los procesos
    int remaining = calib->total_count / calib->work_blocksize; // Por bloques
    int block_total_count = remaining;

    double best_ratio = ratios[0];
    double worst_ratio = ratios[0];
    int best_processor = 0;
    int worst_processor = 0;
    int assigned_blocks = 0;
//  dbglog_info("DEBUG 2000\n");
//  dbglog_info("        Counts (%d): " , remaining);
    for (int i = 0; i < calib->comm_size; i++) {
        //counts
        if (ratios[i] > best_ratio) {
            best_ratio = ratios[i];
            best_processor = i;
        }
//    dbglog_info("DEBUG 2100: best_ratio = %.4f (%d)\n", best_ratio, best_processor);
        if (ratios[i] < worst_ratio) {
            worst_ratio = ratios[i];
            worst_processor = i;
        }
//    dbglog_info("DEBUG 2200: worst_ratio = %.4f (%d)\n", worst_ratio, worst_processor);
//    dbglog_info("DEBUG 2300: count assignation %d\n", calib->counts[i]);
        assigned_blocks = round(block_total_count * ratios[i]);
        calib->counts[i] = assigned_blocks * calib->work_blocksize;
        remaining -= assigned_blocks;
//    dbglog_append("%d ", calib->counts[i]);
//    dbglog_info("DEBUG 2400: remaining counts to assign %d\n", remaining);

//    dbglog_info("DEBUG 2500: displacement assignation %d\n", calib->counts[i]);
        //displs
//    dbglog_info("DEBUG 2600: Proccess %d of %d assigned\n", i, calib->comm_size);
        if (i + 1 < calib->comm_size)
            calib->displs[i + 1] = calib->displs[i] + calib->counts[i];
    }
//  dbglog_info("DEBUG 3000: Remaining = %d\n", remaining);
//  dbglog_append("\n");
    if (remaining > 0)
        calib->counts[best_processor] += remaining * calib->work_blocksize;
    else if (remaining < 0)
        calib->counts[worst_processor] += remaining * calib->work_blocksize;
//  dbglog_info("DEBUG 4000: Finished recalibration\n");

    dbglog_info("         new-counts: ");
    int sum_counts = 0;
    for (int i = 0; i < calib->comm_size; i++) {
        dbglog_append("%d ", calib->counts[i]);
        sum_counts += calib->counts[i];
    } dbglog_append("(%d %lu)\n", sum_counts, calib->total_count);

    return CAL_TAG_RECALIBRATING;
}

static void calibrate(struct cal_calibration* calib) {
    MPI_Gather(&calib->timevalues[calib->comm_rank], 1, MPI_DOUBLE,
            calib->timevalues, 1, MPI_DOUBLE, calib->comm_root, calib->comm);
    if (calib->current_strategy & CAL_STRATEGY_ENERGY) {
        MPI_Gather(&calib->energy->values[calib->comm_rank], 1, MPI_DOUBLE,
                calib->energy->values, 1, MPI_DOUBLE, calib->comm_root,
                calib->comm);
    }

    if (!calib->comm_rank) {
        const int tag = calibrate_root(calib);
        for (int i = 0; i < calib->comm_size; i++)
            MPI_Send(NULL, 0, MPI_BYTE, i, tag, calib->comm);
    }

    MPI_Status status;
    MPI_Recv(NULL, 0, MPI_BYTE, calib->comm_root, MPI_ANY_TAG, calib->comm,
            &status);

    if (status.MPI_TAG == CAL_TAG_RECALIBRATING) {
        MPI_Bcast(calib->counts, calib->comm_size, MPI_INT, calib->comm_root,
                calib->comm);
        MPI_Bcast(calib->displs, calib->comm_size, MPI_INT, calib->comm_root,
                calib->comm);
    } else {
        assert(status.MPI_TAG == CAL_TAG_CALIBRATED);
    }
}

enum cal_error cal_mpi_init() {
#ifdef HAVE_EML
    emlInit();
#endif
    //TODO prepare running calibrations list
    return CAL_SUCCESS;
}

enum cal_error cal_mpi_shutdown(struct cal_calibration *calib) {
    //TODO free resources for running calibrations
#ifdef HAVE_EML
    if (calib->energy->calibrating) {
        calib->energy->calibrating = 0;
        size_t ndevices;
        emlDeviceGetCount(&ndevices);
        emlData_t* data[ndevices];
        emlError_t err;

        err = emlStop(data);
        assert(err == EML_SUCCESS);
        err = emlDataFree(*data);
        assert(err == EML_SUCCESS);
    }

//    emlShutdown();
#endif
    return CAL_SUCCESS;
}

enum cal_error cal_mpi_setup(struct cal_calibration** const newcalib,
        const int* const counts,
        const int* const displs,
        const int work_blocksize,
        enum cal_strategy strategy,
        int total_it,
        int restart_step,
        int restart_value, // simulated annealing style parameter
        const double measurement_interval,
        const double threshold,
        const int root,
        const MPI_Comm comm
) {
    if ((threshold < 0.0) || (threshold > 1.0)) {
        return CAL_INVALID_PARAMETER;
    }
    if (!(strategy & CAL_STRATEGY_TIME) && !(strategy & CAL_STRATEGY_ENERGY)) {
        return CAL_INVALID_PARAMETER;
    }
#ifndef HAVE_EML
    if (strategy & CAL_STRATEGY_ENERGY) {
        return CAL_UNSUPPORTED;
    }
#endif
    struct cal_calibration* const calib = malloc(sizeof(*calib));
    // Intern values
    MPI_Comm_size(comm, &calib->comm_size);
    MPI_Comm_rank(comm, &calib->comm_rank);
    calib->comm_root = root;
    calib->comm = comm;
    calib->strategy = strategy;
    calib->current_strategy = CAL_STRATEGY_NONE;

    // Parameter assignation
    const size_t last = calib->comm_size - 1;
    calib->total_count = displs[last] + counts[last];

    const size_t countsize = sizeof(*calib->counts) * calib->comm_size;
    calib->counts = malloc(countsize);
    memcpy(calib->counts, counts, countsize);

    const size_t displsize = sizeof(*calib->displs) * calib->comm_size;
    calib->displs = malloc(displsize);
    memcpy(calib->displs, displs, displsize);
    calib->work_blocksize = work_blocksize;
    calib->current_iteration = 0;

    calib->threshold = threshold; // XXX MOVE TO TIME STRATEGY
    calib->timevalues = malloc(calib->comm_size * sizeof(double)); // XXX MOVE TO TIME STRATEGY

    // MOVE TO ENERGY SETUP
    if (strategy & CAL_STRATEGY_ENERGY) {
        calib->energy = malloc(sizeof(*calib->energy));
        calib->energy->values = malloc(calib->comm_size * sizeof(double));
        calib->energy->work_per_joule = malloc(
                calib->comm_size * sizeof(double));
        for (int i = 0; i < calib->comm_size; i++)
            calib->energy->work_per_joule[i] = -1.0;

        calib->energy->iteration_interval = 0;
        calib->energy->previous_total_sum = DBL_MAX;
        calib->energy->last_direction = NONE;
        calib->energy->inverted = 0;
        calib->energy->ratio_step = 1 / (double) (calib->comm_size * 2);
        calib->energy->calibrating = 0;
        calib->energy->calibrate_until = 0;
        calib->energy->interval_calc_started = 0;

        calib->energy->total_it = total_it;
        calib->energy->restart_step = restart_step;
        calib->energy->restart_value = restart_value;

        calib->energy->measurement_interval = measurement_interval;
    }

    *newcalib = calib;
    return CAL_SUCCESS;
}

enum cal_error cal_mpi_free(struct cal_calibration* const calib) {
    free(calib->counts);
    free(calib->displs);
    free(calib->timevalues);
    if (calib->strategy & CAL_STRATEGY_ENERGY) {
        free(calib->energy->values);
        free(calib->energy->work_per_joule);
        free(calib->energy);
    }
    free(calib);

    return CAL_SUCCESS;
}

enum cal_error cal_mpi_start(struct cal_calibration* const calib) {
    if (calib->started)
        return CAL_ALREADY_STARTED;

//    dbglog_info("  cal_mpi_start in\n");
    double current_time = MPI_Wtime();
    calib->current_iteration++;
//    dbglog_info("  select_strategy in\n");
    calib->current_strategy = _select_strategy(calib);
//    dbglog_info("  select_strategy out\n");

    //if (calib->current_strategy & CAL_STRATEGY_TIME)
    calib->timevalues[calib->comm_rank] = current_time;

#ifdef HAVE_EML
    if (calib->current_strategy & CAL_STRATEGY_ENERGY) {
        if (calib->energy->iteration_interval != 0 &&
                calib->current_iteration > calib->energy->calibrate_until) {
            // There is no need to check ranges, as cal_mpi_free stops the
            // calibration when the last iteration is reached
            calib->energy->calibrate_until = calib->current_iteration + calib->energy->iteration_interval;
//            dbglog_info("Starting Energy Calibration (rank %d) until %zu\n", calib->comm_rank, calib->energy->calibrate_until);
            calib->energy->calibrating = 1;
            emlError_t err = emlStart();
            assert(err == EML_SUCCESS);
        } else {
            if (!calib->energy->interval_calc_started) {
                calib->energy->interval_calc_started = 1;
                calib->energy->first_energy_calibration_time = calib->timevalues[calib->comm_rank];
            }
        }
    }
#endif
//    dbglog_info("  cal_mpi_start out\n");

    calib->started = 1;
    return CAL_SUCCESS;
}

enum cal_error cal_mpi_stop(struct cal_calibration* const calib,
        int* const counts, int* const displs) {
    if (!calib->started)
        return CAL_NOT_STARTED;

    double* const tvalue = &calib->timevalues[calib->comm_rank];

    *tvalue = MPI_Wtime() - *tvalue;

    memcpy(calib->counts, counts, sizeof(int) * calib->comm_size);
    memcpy(calib->displs, displs, sizeof(int) * calib->comm_size);

#ifdef HAVE_EML
    if (calib->current_strategy & CAL_STRATEGY_ENERGY) {
        if ((calib->energy->iteration_interval != 0) &&
                (calib->current_iteration == calib->energy->calibrate_until)) {
            size_t ndevices;
            emlDeviceGetCount(&ndevices);
            emlData_t* data[ndevices];
            emlError_t err;

            err = emlStop(data);
            assert(err == EML_SUCCESS);

            double* const evalue = &calib->energy->values[calib->comm_rank];
            *evalue = 0;
//      dbglog_info("Iteration %zu, rank %d: consumed ", calib->current_iteration, calib->comm_rank);
            emlDevice_t* dev;
            const char* devname;
            for (size_t i = 0; i < ndevices; i++) {
                double consumed;
                emlDeviceByIndex(i, &dev);
                emlDeviceGetName(dev, &devname);
                err = emlDataGetConsumed(data[i], &consumed);
                assert(err == EML_SUCCESS);
                if (strstr(devname, EML_DEV_TO_USE)) {
                    //        dbglog_append("+ (%s) " DBG_FMT "J ", devname, consumed);
                    *evalue += consumed;
                }
            }
//      dbglog_append(" = " DBG_FMT "J elapsed " DBG_FMT "\n",
//                    *evalue, *tvalue);
//      fflush(stdout);

            err = emlDataFree(*data);
            assert(err == EML_SUCCESS);
            calib->energy->calibrating = 0;
        } else {
            if (INTERVAL_ITERATIONS == calib->current_iteration) {
                double total_time = MPI_Wtime() - calib->energy->first_energy_calibration_time;
                total_time /= INTERVAL_ITERATIONS;
                calib->energy->iteration_interval = floor(calib->energy->measurement_interval / total_time);
                if (!calib->energy->iteration_interval)
                    calib->energy->iteration_interval = 1;
                MPI_Allreduce(&calib->energy->iteration_interval, &calib->energy->iteration_interval, 1, MPI_INT, MPI_MAX, calib->comm);
                if (calib->comm_root == calib->comm_rank)
                    dbglog_info("Calibrating every %zu iterations. CURR_IT: %zu, INTERVAL: %d\n", 
                                calib->energy->iteration_interval, calib->current_iteration, INTERVAL_ITERATIONS);
//                dbglog_info("rank %d MATH: %zu = floor(%.2f / (%.4f / %d))\n", calib->comm_rank, calib->energy->iteration_interval, 
//                            calib->energy->measurement_interval, total_time, INTERVAL_ITERATIONS);
            }
            calib->current_strategy = CAL_STRATEGY_NONE;
        }
    }
#endif
    calibrate(calib);

    memcpy(counts, calib->counts, sizeof(*counts) * calib->comm_size);
    memcpy(displs, calib->displs, sizeof(*displs) * calib->comm_size);

    calib->started = 0;
    return CAL_SUCCESS;
}
