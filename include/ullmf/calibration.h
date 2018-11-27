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
    /** Current Workload */
    ullmf_workload_t* workload;

    // calibration
    double * measurements; // Process last measurements
    int num_procs; // Number of processes
    int id; // Process id
    int root; // Root process
#ifndef NDEBUG
    size_t iteration; // Debug purposes
#endif
    bool started;
    MPI_Comm comm; // MPI communicator
};

extern const void * Ullmf_calibration;

#ifdef __cplusplus
}
#endif


#endif
