/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_strategy_heuristic_energy.c
 * Date: 18 ago. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#include "ullmf/calibration.h"
#include "ullmf/strategy.h"
#include "ullmf/strategy_heuristic.h"
#include "ullmf/strategy_heuristic_energy.h"
#include "ullmf/measurement_device_eml.h"
#include "ullmf/debug.h"
#include <float.h>

// TODO Parameters inside a constructor for the strategy
#define _energy_reset_probability 0.03
#define _energy_reset_increment 0.01
#define _starting_search_distance 0.25
#define _restarting_search_distance 0.08
#define _search_threshold 0.01
#define _max_trials_per_call 3
#define _tolerance 0.95

static ullmf_strategy_heuristic_energy_t _ullmf_strategy_heuristic_energy = {
    .parent.parent._class.size = sizeof(ullmf_strategy_heuristic_energy_t),
    .parent.parent._class.name = ullmf_strategy_heuristic_energy_class,
    .parent.parent.mdevice = (measurement_device_t *) &ullmf_eml_device,
    .parent.parent.calibrate = &ullmf_heuristic_calibrate,
    .parent.parent.redistribute = &ullmf_strategy_redistribute,
	.parent.parent.best_candidate = 0,
	.parent.evalue_workload_distribution = &ullmf_evalue_sum,
	.parent.reset_probability = _energy_reset_probability,
	.parent.initial_reset_probability = _energy_reset_probability,
	.parent.reset_probability_increment = _energy_reset_increment,
	.parent.search_distance = _starting_search_distance,
	.parent.reset_search_distance = _restarting_search_distance,
	.parent.search_threshold = _search_threshold,
    .parent.max_trials_per_call = _max_trials_per_call,
    .parent.moved = false,
    .parent.previous_candidate = 0,
    .parent.previous_consumption = DBL_MAX,
    .parent.are_remaining_movements_last = false,
    .parent.remaining_backtrack_steps = 0,
    .parent.tolerance = _tolerance,
};


ullmf_strategy_t * ullmf_strategy_heuristic_energy = (ullmf_strategy_t *) &_ullmf_strategy_heuristic_energy;
