/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_workload.h
 * Date: 20 jun. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */
#ifndef ULLMF_WORKLOAD_H
#define ULLMF_WORKLOAD_H

#include "ullmf/distribution.h"

#define ullmf_workload_class "workload"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ullmf_workload ullmf_workload_t;

/**
 * Effective distribution of work among different processors:
 * A Workload for 4 processes can be represented as:
 * counts : [1000, 2000, 3000, 4000]
 * displs : [0, 1000, 3000, 6000]
 * num_procs : 4
 * proportional_workload : [0.1, 0.2, 0.3, 0.4]
 * size: 10000
 * blocksize : 2
 */

struct ullmf_workload {
    /**
     * Class size, name, constructor and destructor.
     * costructor(int num_procs, int * counts, int * displs, int blocksize)
     */
    const void * _class;

    /**
     * Total number of processes. Used for array sizes.
     */
    int num_procs;

    /**
     * Array of size num_procs with the amount of work assigned to each process.
     * e.g: [1000, 2000, 3000, 4000]
     */
    int * counts;

    /**
     * Array of size num_procs with location of the counts in the problem data.
     * E.g: displs : [0, 1000, 3000, 6000]
     */
    int * displs;

    /**
     * Normalized workload for each process
     * E.g: proportional_workload : [0.1, 0.2, 0.3, 0.4]
     */
    double * proportional_workload; // Ratios of work assigned to each process
    size_t size; // Total workload
    int blocksize; // Minimum amount of work

    ullmf_workload_t* (* copy)(ullmf_workload_t* self);
    void (* set_workload)(ullmf_workload_t* self, const int * const counts, const int * const displs);
    void (* set_blocksize)(ullmf_workload_t* self, const int block_size);
    ullmf_workload_t* (* new_from_distribution)(ullmf_workload_t* self, ullmf_distribution_t * dist);

};

extern const void * Workload;

// costructor(int num_procs, int * counts, int * displs, int blocksize)

#ifdef __cplusplus
}
#endif

#endif
