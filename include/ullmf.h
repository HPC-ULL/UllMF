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

/**
 * @file
 * UllMF user API.
 * @ingroup api
 */

#include <mpi.h>
#include "ullmf/strategy.h"
#include "ullmf/calibration.h"

#ifdef __cplusplus
extern "C" {
#endif

/** UllMF Calibration procedure errors. */
enum ullmf_error {
    /// Successful operation
    ULLMF_SUCCESS = 0,
    /// Start operation already in progress.
    ULLMF_ALREADY_STARTED,
    /// Start operation has not been called yet
    ULLMF_NOT_STARTED,
    /// Invalid parameter received as input
    ULLMF_INVALID_PARAMETER,
    /// Trying to perform Load balancing without initialization
	ULLMF_NOT_INITIALIZED,
	/// Unsupported Operation
    ULLMF_UNSUPPORTED,
};

/** Initialization for UllMF. Handles the initialization of the strategies and the measurement devices.
 *  @param calib: Calibration control data structure
 *
 *  @return ULLMF_SUCCESS if the initialization was successful.
 */
enum ullmf_error ullmf_mpi_init(ullmf_calibration_t* calib);

/** De-initialization for UllMF. Handles the finalization of the strategies and the measurement devices.
 *  @param calib: Calibration control data structure
 *
 *  @return ULLMF_SUCCESS if the shutdown was successful.
 */
enum ullmf_error ullmf_mpi_shutdown(ullmf_calibration_t* calib);

/** Configures a ullmf_calibration_t, the data structure that controls workload distribution,
 *  workload minimum size (blocksize) and workload distribution strategy. It also defines the
 *  root process and the MPI Communicator.
 *  @param[in] calib: ullmf_calibration_t control data structure to initialize.
 *  @param[in] counts: Amount of work assigned to each process.
 *  @param[in] displs: Displacement vector with the location of the counts assigned to each process.
 *  @param[in] blocksize: Minimum step to move counts between process (amount of counts per unit of work).
 *  @param[in] strategy: Chosen strategy to perform the dynamic load balancing.
 *  @param[in] root: Process assigned to manage the load balancing logic.
 *  @param[in] comm: MPI Communicator to be used inside the dynamic load balancing procedures.
 *
 *  @retval ULLMF_SUCCESS if the setup was performed successfully.
 */
enum ullmf_error ullmf_mpi_setup(ullmf_calibration_t** const new_calib,
        const int* const counts,
		const int* const displs,
		const int blocksize,
        ullmf_strategy_t* const strategy,
		const int root,
        const MPI_Comm comm);

/** Deallocates all the dynamic memory within the ullmf_calibration_t data structure
 * @param[in] calib: ullmf_calibration_t control data structure.
 *
 *  @retval ULLMF_SUCCESS if the free was performed successfully.
 */
enum ullmf_error ullmf_mpi_free(ullmf_calibration_t* calib);

/** Starts the calibration phase
 *  @param[in] calib: ullmf_calibration_t control data structure.
 *
 *  @retval ULLMF_SUCCESS if the calibration process was performed successfully.
 */
enum ullmf_error ullmf_mpi_start(ullmf_calibration_t* calib);

/** Stops the calibration phase. Uses the measurements gathered since the last ullmf_mpi_start call
 *  to perform the dynamic load balancing strategy defined in the setup phase.
 *  Determines the new workload distribution, which gets returned inside counts and displs.
 *  @param[in] calib: ullmf_calibration_t control data structure.
 *
 *  @param[out] counts: Amounts of data assigned to each process.
 *  @param[out] displs: Displacements Vector from the data base address for each process.
 *
 *  @retval ULLMF_SUCCESS if the calibration process was performed successfully.
 */
enum ullmf_error ullmf_mpi_stop(ullmf_calibration_t* calib, int* counts, int* displs);

#ifdef __cplusplus
}
#endif

#endif
