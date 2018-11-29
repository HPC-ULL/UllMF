/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_calibration.h
 * Date: 20 jun. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#ifndef ULLMF_CALIBRATION_H
#define ULLMF_CALIBRATION_H

#include <mpi.h>
#include <stdbool.h>

typedef struct ullmf_calibration ullmf_calibration_t;

#include "ullmf/class_utils.h"
#include "ullmf/strategy.h"
#include "ullmf/workload.h"

#define ullmf_calibration_class "ullmf_calibration"

#ifdef __cplusplus
extern "C" {
#endif


/** Contains state, properties and methods for an strategy */
struct ullmf_calibration {
    /** Object inheritance */
    class_t _class;

    /** Calibration strategy */
    ullmf_strategy_t* strategy;

    /** Current Workload Distribution */
    ullmf_workload_t* workload;

    /** Last measurements gathered for the processes */
    double * measurements;

    /** Total number of processes in the parallel execution */
    int num_procs;

    /** Current Process Id */
    int id;

    /** Root/Master process that manages the workload distribution logic */
    int root; // Root process

#ifndef NDEBUG
    /** Current Iteration inside the calibration. Used only for Debug purposes. */
    size_t iteration; // Debug purposes
#endif

    /** Internal State of the Calibration Procedures.
     *  Started == 1 means ullmf_mpi_start has been called.
     *  Resets to 0 when ullmf_mpi_stop is called.
     */
    bool started;

    /** MPI communicator */
    MPI_Comm comm;
};

extern const void * Ullmf_calibration;

#ifdef __cplusplus
}
#endif


#endif
