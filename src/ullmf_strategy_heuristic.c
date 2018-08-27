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

#include <math.h>

double _ullmf_evalue_sum(ullmf_calibration_t* calib,
					     int* candidate_counts, double* resource_ratios) {
	double evaluation = 0;
	for (int i = 0; i < calib->num_procs; i++) {
		evaluation += candidate_counts[i] * resource_ratios[i];
	}
	return evaluation;
}

double _ullmf_evalue_max(ullmf_calibration_t* calib,
					     int* candidate_counts, double* resource_ratios) {
	double evaluation = 0;
	for (int i = 0; i < calib->num_procs; i++) {
		evaluation += candidate_counts[i] * resource_ratios[i];
	}
	return evaluation;
}

int _ullmf_heuristic_calibrate(ullmf_calibration_t* calib) {
	if (calib->strategy->mdevice->measuring) // Energy measurements require time
		return ULLMF_TAG_CALIBRATED;

	ullmf_strategy_heuristic_t* strategy = (ullmf_strategy_heuristic_t*) calib->strategy;
	if (strategy->search_distance < strategy->search_threshold) {
		//Probability
		bool reset = (double)(random() % 10000) / 100;
		if (reset) {
			strategy->search_distance = strategy->reset_distance;
			strategy->threshold // Listing 3.4
		} else {
			return ULLMF_TAG_CALIBRATED;
		}

	}

	return ULLMF_TAG_RECALIBRATING;
}
