/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_calibation.h
 * Date: 20 jun. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#ifndef ULLMF_CALIBRATION_H
#define ULLMF_CALIBRATION_H

#include <mpi.h>
#include <stdbool.h>

typedef struct ullmf_calibration ullmf_calibration_t;

#include "ullmf_class_utils.h"
#include "ullmf_strategy.h"
#include "ullmf_workload.h"

#ifdef __cplusplus
extern "C" {
#endif


/** Contains state, properties and methods for an strategy */
// TODO separar ull_calibration_t en ull_calibration_t y ull_workload_t
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
    bool started;
    MPI_Comm comm; // MPI communicator
};


#ifdef __cplusplus
}
#endif


#endif
