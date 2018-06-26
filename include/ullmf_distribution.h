/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_distribution.h
 * Date: 26 jun. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */
#ifndef ULLMF_CANDIDATE_H
#define ULLMF_CANDIDATE_H

#define ullmf_candidate_class "candidate"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ullmf_distribution ullmf_distribution_t;

struct ullmf_distribution {
    const void * _class;
    int num_procs; // number of candidates
    double * ratios; // Workload normalized distribution
    double total; // Workload total (to check if sum(ratios) == 1

    void (* set_ratios)(ullmf_distribution_t * self, const int num_procs, const double * const ratios);
    int (* get_num_procs)(ullmf_distribution_t * self);
    double (* get_total)(ullmf_distribution_t * self);
};

extern const void * Distribution;

#ifdef __cplusplus
}
#endif

#endif
