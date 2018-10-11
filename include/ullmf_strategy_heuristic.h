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
    double reset_probability_increment;

    /** Neighbor Candidate distance */
	double search_distance;

	/** Neighbor Candidate distance when reset probability is met  */
	double reset_search_distance;

	/** Threshold to stop the heuristic search */
	double search_threshold;

	/** Did I find a solution to move in the last iteration */
	bool moved;

	/** Inversion mechanism for last movement */
	bool tried_inversion;

	/** Was the previous movement an inversion */
	bool previously_inverted; //TODO

	/** Previous resource consumption */
	double previous_consumption;

    /** Heuristic population evaluation */
    double (*evalue_workload_distribution)(ullmf_calibration_t* calib,
    									ullmf_workload_t* candidate_counts, double* resource_ratios);
};


double ullmf_evalue_sum(ullmf_calibration_t* calib,
						ullmf_workload_t* candidate, double* resource_ratios);

double ullmf_evalue_max(ullmf_calibration_t* calib,
						ullmf_workload_t* candidate, double* resource_ratios);

bool is_movement_legal(double current_ratio, double ratio_step, int direction);

double get_resource_ratio(double resource_consumption, int counts);

ullmf_workload_t * move_workload(ullmf_calibration_t* calib, int process, int direction);

int generate_distributions(ullmf_calibration_t* calib, ullmf_workload_t*** candidates);

void free_distributions(int num_candidates, ullmf_workload_t*** candidates);

void heuristic_search(ullmf_calibration_t* calib);

int ullmf_heuristic_calibrate(ullmf_calibration_t* calib);





#ifdef __cplusplus
}
#endif


#endif
