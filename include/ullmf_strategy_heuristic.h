/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_strategy_heuristic.h
 * Date: 20 jun. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#ifndef ULLMF_STRATEGY_HEURISTIC_H
#define ULLMF_STRATEGY_HEURISTIC_H

#include <stdbool.h>

#include "ullmf_class_utils.h"
#include "ullmf_calibration.h"
#include "ullmf_strategy.h"


#ifdef __cplusplus
extern "C" {
#endif

#define ullmf_strategy_heuristic_class "ullmf_strategy_heuristic"

typedef struct ullmf_strategy_heuristic ullmf_strategy_heuristic_t;

/** Contains state, properties and methods for an strategy */
struct ullmf_strategy_heuristic {
    /** Object inheritance */
    ullmf_strategy_t parent;

    /** Determines whether the measurement is being done or not */
    bool is_calibrating;

    // TODO Use Int logic instead of doubles for probabilities
    /** 0 - 10000 value representing  a probability from 0.00% to 100.00% */
    double reset_probability;

    /** Used to save the value of reset_probability though the algorithm
     * 0 - 10000 value representing  a probability from 0.00% to 100.00% */
    double initial_reset_probability;

    /** Amount of probability to increment in the heuristic after a criteria is met */
    double reset_increment;

    /** Neighbor Candidate distance */
	double search_distance;

	/** Neighbor Candidate distance when reset probability is met  */
	double reset_distance;

	/** Threshold to stop the heuristic search */
	double search_threshold;

    /** Heuristic population evaluation */
    double (*evalue_workload_distribution)(ullmf_calibration_t* calib,
    									   int* candidate_counts, double* resource_ratios);
};

double _ullmf_evalue_sum(ullmf_calibration_t* calib,
						 int* candidate_counts, double* resource_ratios);

double _ullmf_evalue_max(ullmf_calibration_t* calib,
						 int* candidate_counts, double* resource_ratios);

int _ullmf_heuristic_calibrate(ullmf_calibration_t* calib);


#ifdef __cplusplus
}
#endif


#endif
