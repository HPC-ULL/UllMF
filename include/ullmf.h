/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_strategy_calibrate.c
 * Date: 15 jun. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#ifndef ULLMF_H
#define ULLMF_H

#include <mpi.h>
#include "ullmf/strategy.h"
#include "ullmf/calibration.h"

#ifdef __cplusplus
extern "C" {
#endif

enum ullmf_error {
    ULLMF_SUCCESS = 0,
    ULLMF_ALREADY_STARTED,
    ULLMF_NOT_STARTED,
    ULLMF_INVALID_PARAMETER,
	ULLMF_NOT_INITIALIZED,
    ULLMF_UNSUPPORTED,
};

enum ullmf_error ullmf_mpi_init(ullmf_calibration_t* calib);
enum ullmf_error ullmf_mpi_shutdown(ullmf_calibration_t* calib);

enum ullmf_error ullmf_mpi_setup(ullmf_calibration_t** const new_calib,
        const int* const counts,
		const int* const displs,
		const int blocksize,
        ullmf_strategy_t* const strategy,
		const int root,
        const MPI_Comm comm);
enum ullmf_error ullmf_mpi_free(ullmf_calibration_t* calib);

enum ullmf_error ullmf_mpi_start(ullmf_calibration_t* calib);
enum ullmf_error ullmf_mpi_stop(ullmf_calibration_t* calib, int* counts, int* displs);

#ifdef __cplusplus
}
#endif

#endif
