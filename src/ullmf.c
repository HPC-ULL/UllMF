/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf.c
 * Date: 20 jun. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#include <mpi.h>

#include "ullmf.h"
#include "ullmf_calibration.h"

enum ullmf_error ullmf_mpi_init() {
    return ULLMF_SUCCESS;
}

enum ullmf_error ullmf_mpi_shutdown(ullmf_calibration_t* const calib) {
    return ULLMF_SUCCESS;
}

enum ullmf_error ullmf_mpi_setup(ullmf_calibration_t** const new_calib,
        const int* const counts,
        const int* const displs,
        const ullmf_strategy_t* strategy,
        const int root,
        const MPI_Comm comm) {

    ullmf_calibration_t* const calib = malloc(sizeof(*calib));
    MPI_Comm_size(comm, &calib->num_procs);
    MPI_Comm_rank(comm, &calib->id);
    calib->comm = comm;
    calib->root = root;
    calib->strategy = strategy;

    size_t memsize = sizeof(*calib->counts) * calib->num_procs;
    calib->counts = malloc(memsize);
    memcpy(calib->counts, counts, memsize);

    memsize = sizeof(*calib->displs) * calib->num_procs;
    calib->displs = malloc(memsize);
    memcpy(calib->displs, displs, memsize);

    calib->workload_size = displs[calib->num_procs - 1] + counts[calib->num_procs - 1];

    calib->measurements = malloc(calib->num_procs * sizeof(double));
    *new_calib = calib;
    return ULLMF_SUCCESS;
}

enum ullmf_error ullmf_mpi_free(ullmf_calibration_t* const calib) {
    free(calib->counts);
    free(calib->displs);
    free(calib->measurements);
    return ULLMF_SUCCESS;
}

enum ullmf_error ullmf_mpi_start(ullmf_calibration_t* const calib) {
    if (calib->started)
        return ULLMF_ALREADY_STARTED;

    calib->strategy->mdevice->measurement_start();
    return ULLMF_SUCCESS;
}

enum ullmf_error ullmf_mpi_stop(ullmf_calibration_t* const calib, int* counts, int* displs){
    return ULLMF_SUCCESS;
}

