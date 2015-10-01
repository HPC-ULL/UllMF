#ifndef CALIBRATE_MPI_H
#define CALIBRATE_MPI_H

#include <mpi.h>

#ifdef __cplusplus
extern "C" {
#endif

enum cal_error {
	CAL_SUCCESS = 0,
	CAL_ALREADY_STARTED,
	CAL_NOT_STARTED,
	CAL_INVALID_PARAMETER,
	CAL_UNSUPPORTED,
};

enum cal_strategy {
	CAL_STRATEGY_NONE = 0,
	CAL_STRATEGY_TIME = 1,
	CAL_STRATEGY_ENERGY = 2,
};

typedef struct cal_calibration cal_calibration_t;

enum cal_error cal_mpi_init();
enum cal_error cal_mpi_shutdown(struct cal_calibration *calib);

enum cal_error cal_mpi_setup(
		cal_calibration_t** calib,
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
		const MPI_Comm comm);
enum cal_error cal_mpi_free(cal_calibration_t* calib);

enum cal_error cal_mpi_start(cal_calibration_t* calib);
enum cal_error cal_mpi_stop(cal_calibration_t* calib, int* counts, int* displs);

#define MIN_TIME_ELAPSED_DIFF 0.005 // at least 5 milliseconds of difference 

#ifdef __cplusplus
}
#endif

#endif
