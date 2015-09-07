#include <assert.h>
#include "debug.h"
#include <float.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_EML
#include <eml.h>
#endif

#include <mpi.h>

#include "calibrate_mpi.h"

#define DBG_FMT "%8g"

struct cal_calibration {
	MPI_Comm comm;
	int comm_size;
	int comm_rank;
	int comm_root;
	double efficiency_weight;
	double threshold;
	size_t total_count;

	double* timevalues;
	double* energyvalues;
	bool started;
	int* counts;
	int* displs;
};

const int CAL_TAG_RECALIBRATING = 1;
const int CAL_TAG_CALIBRATED = 2;

static int calibrate_root(const struct cal_calibration* const calib) {
	double tmax = DBL_MIN;
	double tmin = DBL_MAX;
	double taccum = 0;
	for (int i = 0; i < calib->comm_size; i++) {
		taccum += calib->timevalues[i];
		if (calib->timevalues[i] == 0)
			continue;
		if (calib->timevalues[i] > tmax)
			tmax = calib->timevalues[i];
		if (calib->timevalues[i] < tmin)
			tmin = calib->timevalues[i];
	}

	int most_efficient_proc = -1;
	if (calib->efficiency_weight != 0.0) {
		double effmin = DBL_MAX;
		for (int i = 0; i < calib->comm_size; i++) {
			if (calib->energyvalues[i] == 0)
				continue;
			const double eff = calib->energyvalues[i] / calib->counts[i];
			if (eff < effmin) {
				effmin = eff;
				most_efficient_proc = i;
			}
		}
		assert(most_efficient_proc != -1);
	}
	else {
		//TODO make this check work with EML too
		//do nothing if calibration threshold was already reached
		if ((tmax == DBL_MIN) || (tmin == DBL_MAX) ||
			((1 - tmin/tmax) < calib->threshold))
			return CAL_TAG_CALIBRATED;
	}

	dbglog_info("    raw time values: ");
	for (int i = 0; i < calib->comm_size; i++) {
		dbglog_append(DBG_FMT " ", calib->timevalues[i]);
	}
	dbglog_append("\n");

	dbglog_info("        time ratios: ");
	double ratios[calib->comm_size];
	double ratio_sum = 0;
	for (int i = 0; i < calib->comm_size; i++) {
		if (calib->timevalues[i] == 0)
			ratios[i] = 0;
		else
			ratios[i] = calib->counts[i] / calib->timevalues[i];

		dbglog_append(DBG_FMT " ", ratios[i]);
		ratio_sum += ratios[i];
	}
	dbglog_append("\n");

	if (calib->efficiency_weight != 0.0) {
		dbglog_info("eff-adjusted ratios: ");
		ratio_sum = 0;
		for (int i = 0; i < calib->comm_size; i++) {
			if (i != most_efficient_proc)
				ratios[i] *= 1 - calib->efficiency_weight;

			dbglog_append(DBG_FMT " ", ratios[i]);
			ratio_sum += ratios[i];
		}
		dbglog_append("\n");
	}

	dbglog_info("  normalized ratios: ");
	int remaining = calib->total_count;
	double best_ratio = ratios[0] / ratio_sum;
	double worst_ratio = ratios[0] / ratio_sum;
	int best_processor = 0;
	int worst_processor = 0;
	for (int i = 0; i < calib->comm_size; i++) {
		//counts
		ratios[i] /= ratio_sum;
		dbglog_append(DBG_FMT " ", ratios[i]);
		if (ratios[i] > best_ratio) {
			best_ratio = ratios[i];
			best_processor = i;
		}
		if (ratios[i] < worst_ratio) {
			worst_ratio = ratios[i];
			worst_processor = i;
		}

		calib->counts[i] = round(calib->total_count * ratios[i]);
		remaining -= calib->counts[i];

		//displs
		if (i+1 < calib->comm_size)
			calib->displs[i+1] = calib->displs[i] + calib->counts[i];
	}
	dbglog_append("\n");
	if (remaining > 0)
		calib->counts[best_processor] += remaining;
	else if (remaining < 0)
		calib->counts[worst_processor] += remaining;

	return CAL_TAG_RECALIBRATING;
}

static void calibrate(const struct cal_calibration* const calib) {
	MPI_Gather(&calib->timevalues[calib->comm_rank], 1, MPI_DOUBLE,
		calib->timevalues, 1, MPI_DOUBLE,
		calib->comm_root, calib->comm);
	MPI_Gather(&calib->energyvalues[calib->comm_rank], 1, MPI_DOUBLE,
		calib->energyvalues, 1, MPI_DOUBLE,
		calib->comm_root, calib->comm);

	if (!calib->comm_rank) {
		const int tag = calibrate_root(calib);
		for (int i = 0; i < calib->comm_size; i++)
			MPI_Send(NULL, 0, MPI_BYTE, i, tag, calib->comm);
	}

	MPI_Status status;
	MPI_Recv(NULL, 0, MPI_BYTE, calib->comm_root, MPI_ANY_TAG, calib->comm, &status);

	if (status.MPI_TAG == CAL_TAG_RECALIBRATING) {
		MPI_Bcast(calib->counts, calib->comm_size, MPI_INT, calib->comm_root, calib->comm);
		MPI_Bcast(calib->displs, calib->comm_size, MPI_INT, calib->comm_root, calib->comm);
	}
	else {
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

enum cal_error cal_mpi_shutdown() {
	//TODO free resources for running calibrations
#ifdef HAVE_EML
	emlShutdown();
#endif
	return CAL_SUCCESS;
}

enum cal_error cal_mpi_setup(
	struct cal_calibration** const newcalib,
	const int* const counts,
	const int* const displs,
	const double efficiency_weight,
	const double threshold,
	const int root,
	const MPI_Comm comm)
{
	if ((threshold < 0.0) || (threshold > 1.0))
		return CAL_INVALID_PARAMETER;
	if ((efficiency_weight < 0.0) || (efficiency_weight > 1.0))
		return CAL_INVALID_PARAMETER;
#ifndef HAVE_EML
	if (efficiency_weight != 0.0)
		return CAL_UNSUPPORTED;
#endif

	struct cal_calibration* const calib = malloc(sizeof(*calib));

	calib->comm = comm;
	MPI_Comm_size(comm, &calib->comm_size);
	MPI_Comm_rank(comm, &calib->comm_rank);
	calib->comm_root = root;

	calib->counts = malloc(calib->comm_size * sizeof(*calib->counts));
	calib->displs = malloc(calib->comm_size * sizeof(*calib->displs));
	calib->timevalues = malloc(calib->comm_size * sizeof(*calib->timevalues));
	calib->energyvalues = malloc(calib->comm_size * sizeof(*calib->energyvalues));

	calib->efficiency_weight = efficiency_weight;
	calib->threshold = threshold;

	const size_t last = calib->comm_size - 1;
	calib->total_count = displs[last] + counts[last];

	const size_t countsize = sizeof(*calib->counts) * calib->comm_size;
	calib->counts = malloc(countsize);
	memcpy(calib->counts, counts, countsize);

	const size_t displsize = sizeof(*calib->displs) * calib->comm_size;
	calib->displs = malloc(displsize);
	memcpy(calib->displs, displs, displsize);

	calib->started = false;
	*newcalib = calib;
	return CAL_SUCCESS;
}

enum cal_error cal_mpi_free(struct cal_calibration* const calib) {
	free(calib->counts);
	free(calib->displs);
	free(calib->timevalues);
	free(calib->energyvalues);
	free(calib);

	return CAL_SUCCESS;
}

enum cal_error cal_mpi_start(struct cal_calibration* const calib) {
	if (calib->started)
		return CAL_ALREADY_STARTED;

	calib->timevalues[calib->comm_rank] = MPI_Wtime();

#ifdef HAVE_EML
	if (calib->efficiency_weight > 0.0) {
		emlError_t err = emlStart();
		assert(err == EML_SUCCESS);
	}
#endif

	calib->started = true;
	return CAL_SUCCESS;
}

enum cal_error cal_mpi_stop(
	struct cal_calibration* const calib,
	int* const counts,
	int* const displs)
{
	if (!calib->started)
		return CAL_NOT_STARTED;

	double* const tvalue = &calib->timevalues[calib->comm_rank];

	*tvalue = MPI_Wtime() - *tvalue;

#ifdef HAVE_EML
	if (calib->efficiency_weight > 0.0) {
		size_t ndevices;
		emlDeviceGetCount(&ndevices);
		emlData_t* data[ndevices];
		emlError_t err;

		err = emlStop(data);
		assert(err == EML_SUCCESS);

		double* const evalue = &calib->energyvalues[calib->comm_rank];
		*evalue = 0;
		dbglog_info("rank %d: consumed ", calib->comm_rank);
		for (size_t i = 0; i < ndevices; i++) {
			double consumed;
			err = emlDataGetConsumed(data[i], &consumed);
			assert(err == EML_SUCCESS);
			dbglog_append("+" DBG_FMT, consumed);
			*evalue += consumed;
		}

		err = emlDataGetElapsed(data[0], tvalue);
		assert(err == EML_SUCCESS);
		dbglog_append(" = " DBG_FMT " elapsed " DBG_FMT "\n",
			*evalue, *tvalue);

		err = emlDataFree(*data);
		assert(err == EML_SUCCESS);
	}
#endif

	calibrate(calib);

	memcpy(counts, calib->counts, sizeof(*counts) * calib->comm_size);
	memcpy(displs, calib->displs, sizeof(*displs) * calib->comm_size);

	calib->started = false;
	return CAL_SUCCESS;
}
