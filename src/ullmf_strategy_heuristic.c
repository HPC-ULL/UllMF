/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_strategy_heuristic.c
 * Date: 23 jul. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#include "ullmf_strategy_heuristic.h"
#include "ullmf_calibration.h"
#include "ullmf_utils.h"
#include "debug.h"

#include <math.h>
#include <float.h>
#include <string.h>
#include <stdbool.h>

static const float epsilon = 1e-9;


double ullmf_evalue_sum(ullmf_calibration_t* calib,
						ullmf_workload_t* candidate, double* resource_ratios) {
	double evaluation = 0;
	for (int i = 0; i < calib->num_procs; i++) {
		evaluation += candidate->counts[i] * resource_ratios[i];
	}
	return evaluation;
}


double ullmf_evalue_max(ullmf_calibration_t* calib,
						ullmf_workload_t* candidate, double* resource_ratios) {
	double evaluation = 0;
	for (int i = 0; i < calib->num_procs; i++) {
		double max = candidate->counts[i] * resource_ratios[i];
		if (max > evaluation)
			evaluation = max;
	}
	return evaluation;
}


bool is_movement_legal(double current_ratio, double ratio_step, int direction) {
    if ((current_ratio + ratio_step * direction) >= 0
            && (current_ratio + ratio_step * direction) <= 1)
        return true;
    return false;
}


double get_resource_ratio(double resource_consumption, int counts) {
    if (resource_consumption < epsilon) // <= 0
    	return 0;
    else
    	return resource_consumption / (double) counts;
}


ullmf_workload_t * move_workload(ullmf_calibration_t* calib, int process, int direction) {
	ullmf_strategy_heuristic_t * heuristic = (ullmf_strategy_heuristic_t *) calib->strategy;
	double to_move = heuristic->search_distance;
	size_t memsize = calib->num_procs * sizeof(double);
	double * new_proportional_workload = malloc(memsize);
	memcpy(new_proportional_workload, calib->workload->proportional_workload, memsize);
	new_proportional_workload[process] += to_move * direction;
	double remaining_movement = to_move;
	while (!f_is_zero(remaining_movement, epsilon)) {
		double current_movement = remaining_movement / (calib->num_procs - 1);
		for (int i = 0; i < calib->num_procs; i++) {
			if (i == process)
				continue;
			if (is_movement_legal(new_proportional_workload[i], current_movement, -direction)) {
				new_proportional_workload[i] += current_movement * -direction;
				remaining_movement -= current_movement;
			}
		}
	}

	ullmf_distribution_t * new_distribution = _new(Distribution, calib->num_procs, new_proportional_workload);
	ullmf_workload_t * new_workload = calib->workload->new_from_distribution(calib->workload, new_distribution);
	_delete(new_distribution);
	free(new_proportional_workload);
	return new_workload;
}


int generate_distributions(ullmf_calibration_t* calib, ullmf_workload_t*** candidates) {

	ullmf_strategy_heuristic_t * heuristic = (ullmf_strategy_heuristic_t *) calib->strategy;
	int num_candidates = calib->num_procs * 2; // TODO substitution for parametric value heuristic size
                                               	   // should be included inside heuristic constructor
	int direction = 1;
	int switch_direction = 0;
	(*candidates) = malloc(num_candidates * sizeof(*candidates));
	int current_candidate = 0;
	for (int i = 0; i < num_candidates; i++) {
		int process = i % calib->num_procs;
		if (is_movement_legal(calib->workload->proportional_workload[process],
				heuristic->search_distance, direction)) {
			(*candidates)[current_candidate++] = move_workload(calib, process, direction);
		}
		switch_direction += 1;
		if (switch_direction == calib->num_procs) {
			switch_direction = 0;
			direction = -direction;
		}

	}

	return current_candidate;
}


void free_distributions(int num_candidates, ullmf_workload_t*** candidates) {
	for (int i = 0; i < num_candidates; i++)
		_delete((*candidates)[i]);
	free(*candidates);
	*candidates = 0;
}


void heuristic_search(ullmf_calibration_t* calib) {
	ullmf_strategy_heuristic_t * heuristic = (ullmf_strategy_heuristic_t *) calib->strategy;
	// TODO Inversion if last solution is better than current one
	if (heuristic->moved && !heuristic->tried_inversion) {
		heuristic->tried_inversion = true;
		// TODO try invert
		return;
	}

	// Calculate resources per unit of work
	double * resource_ratios = calloc(calib->num_procs, sizeof(double));
    for (int i = 0; i < calib->num_procs; i++)
    	resource_ratios[i] = get_resource_ratio(calib->measurements[i], calib->workload->counts[i]);

    // Generate heuristic population of candidates
    ullmf_workload_t ** candidates;
    int num_candidates = generate_distributions(calib, &candidates);

    // Evaluate heuristic population
	if (calib->strategy->best_candidate != 0)
		_delete(calib->strategy->best_candidate);

    calib->strategy->best_candidate = _new(Distribution, calib->num_procs, calib->workload->proportional_workload);
	heuristic->moved = false;
    heuristic->tried_inversion = false;
    double best_consumption = heuristic->evalue_workload_distribution(calib, calib->workload, resource_ratios);
    double candidate_consumption;
    for (int i = 0; i < num_candidates; i++) {
    	candidate_consumption = heuristic->evalue_workload_distribution(calib, candidates[i], resource_ratios);
    	if (candidate_consumption < best_consumption) {
    		heuristic->moved = true;
    		candidate_consumption = best_consumption;
    		calib->strategy->best_candidate->set_proportional_workload(
    				calib->strategy->best_candidate, candidates[i]->proportional_workload);
    	}
    }

    heuristic->search_distance /= 2;
    free_distributions(num_candidates, &candidates);
    free(resource_ratios);
}


int ullmf_heuristic_calibrate(ullmf_calibration_t* calib) {
	if (calib->strategy->mdevice->measuring) // Energy measurements require time
		return ULLMF_TAG_CALIBRATED;

	ullmf_strategy_heuristic_t* heuristic = (ullmf_strategy_heuristic_t*) calib->strategy;
	if (heuristic->search_distance < heuristic->search_threshold) {
		//Probability
		if (heuristic->moved && !heuristic->tried_inversion) {
			heuristic->tried_inversion = true;
			// TODO try invert
			return ULLMF_TAG_RECALIBRATING;
		} else {
			double reset = (double)(random() % 10000) / 10000; // TODO change to int logic
			if (reset < heuristic->reset_probability) {
				heuristic->search_distance = heuristic->reset_search_distance;
				heuristic->reset_probability = heuristic->initial_reset_probability;
			} else {
				heuristic->reset_probability += heuristic->reset_probability_increment;
				return ULLMF_TAG_CALIBRATED;
			}
		}
	}

	heuristic_search(calib);

	return ULLMF_TAG_RECALIBRATING;
}
