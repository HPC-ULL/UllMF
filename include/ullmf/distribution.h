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

#ifndef ULLMF_DISTRIBUTION_H
#define ULLMF_DISTRIBUTION_H

#define ullmf_candidate_class "distribution"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ullmf_distribution ullmf_distribution_t;

/**
 * Proportional distribution of work among different processors:
 * A Workload of [1000,1000,1000,1000] for 4 processes is translated
 * to a distribution of [0.25, 0.25, 0.25, 0.25] for normalization insidie
 * the dynamic load balancing procedures.
 */
struct ullmf_distribution {
    /**
     * Class size, name, constructor and destructor.
     * constructor(int num_procs, double * ratios)
     */
    const void * _class;

    /**
     * Total number of processes. Used for array sizes.
     */
    int num_procs;

    /**
     * Proportional workload in the form [0.25, 0.50, 0.25], where 0 <= proportional_workload[i] <= 1
     */
    double * proportional_workload; // Workload normalized distribution

    /**
     * Sum Failcheck for the proportional workload. sum(ratios) == 1
     */
    double total;

    /**
     * Excess (or lack of excess) to be able to distribute remainders if total != 1.
     * Excess is used inside the logic of the object, while total is just a failcheck.
     * Both variables are calculated independently.
     */
    double excess;

    /**
     * Proportional Workload Setter.
     * @param self: object
     * @param proportional_workload: new proportional workload to copy
     */
    void (* set_proportional_workload)(ullmf_distribution_t * self,
    								   const double * const proportional_workload);

    /**
     * get_num_procs.
     * @param self: object
     * @param proportional_workload: new proportional workload to copy
     */
    int (* get_num_procs)(ullmf_distribution_t * self);

    /**
     * Proportional Workload Setter.
     * @param self: object
     * @param proportional_workload: new proportional workload to copy
     */
    double (* get_total)(ullmf_distribution_t * self);

    /**
     * Redistributes the excess of the object if excess > 0 (Which implies total != 1).
     * @param self: object
     * @param proportional_workload: new proportional workload to copy
     */
    void (* redistribute_remainder)(ullmf_distribution_t * self);
};

extern const void * Distribution;


#ifdef __cplusplus
}
#endif

#endif
