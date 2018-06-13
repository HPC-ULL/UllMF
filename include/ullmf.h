#ifndef ULLMF_H
#define ULLMF_H

#include <mpi.h>
#include "ullmf_strategy.h"

#ifdef __cplusplus
extern "C" {
#endif

enum ullmf_error {
    ULLMF_SUCCESS = 0,
    ULLMF_ALREADY_STARTED,
    ULLMF_NOT_STARTED,
    ULLMF_INVALID_PARAMETER,
    ULLMF_UNSUPPORTED,
};

typedef struct ullmf_calibration ullmf_calibration_t;

enum ullmf_error ullmf_mpi_init();
enum ullmf_error ullmf_mpi_shutdown(struct cal_calibration *calib);

enum ullmf_error ullmf_mpi_setup(ullmf_calibration_t* calib,
        const int* const counts, const int* const displs,
        const enum ullmf_strategy_t* strategy, const int root,
        const MPI_Comm comm);
enum ullmf_error ullmf_mpi_free(ullmf_calibration_t* calib);

enum ullmf_error ullmf_mpi_start(ullmf_calibration_t* calib);
enum ullmf_error ullmf_mpi_stop(ullmf_calibration_t* calib, int* counts,
        int* displs);

#ifdef __cplusplus
}
#endif

#endif
