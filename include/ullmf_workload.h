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

#define ullmf_workload_class "workload"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ullmf_workload ullmf_workload_t;

struct ullmf_workload {
    const void * _class;
    int * counts; // Amount of workload
    int * displs; // Workload location
    int size; // Total workload
    int blocksize; // Minimum amount of work
    int num_procs; // Num processes

    void (* set_workload)(ullmf_workload_t* self, const int * const counts, const int * const displs);
    void (* set_blocksize)(ullmf_workload_t* self, const int block_size);
};

extern const void * Workload;

#ifdef __cplusplus
}
#endif

#endif
