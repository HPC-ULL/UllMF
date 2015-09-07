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

typedef struct cal_calibration cal_calibration_t;

enum cal_error cal_mpi_init();
enum cal_error cal_mpi_shutdown();

enum cal_error cal_mpi_setup(
	cal_calibration_t** calib,
	const int* counts,
	const int* displs,
	double efficiency_weight,
	double threshold,
	int root,
	const MPI_Comm comm);
enum cal_error cal_mpi_free(cal_calibration_t* calib);

enum cal_error cal_mpi_start(cal_calibration_t* calib);
enum cal_error cal_mpi_stop(cal_calibration_t* calib, int* counts, int* displs);

#ifdef __cplusplus
}
#endif

#endif
