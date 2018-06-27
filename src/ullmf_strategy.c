/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_strategy.c
 * Date: 20 jun. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#include "ullmf_calibration.h"
#include "ullmf_distribution.h"
#include "ullmf_strategy.h"
#include "debug.h"

#include <math.h>

void ullmf_strategy_redistribute(ullmf_calibration_t* calib) {
	ullmf_distribution_t * new_distribution = calib->strategy->best_candidate;
	ullmf_workload_t * workload = calib->workload;

	int block_total_count = workload->size / workload->blocksize;
	int remaining = block_total_count;

	double best_ratio = new_distribution->ratios[0];
	double worst_ratio = new_distribution->ratios[0];

	int best_processor = 0;
	int worst_processor = 0;
	int assigned_blocks = 0;

	// Transforms ratios into real problem sizes
	for (int i = 0; i < workload->num_procs; i++) {
		if (new_distribution->ratios[i] > best_ratio) {
			best_ratio = new_distribution->ratios[i];
			best_processor = i;
		}

		if (new_distribution->ratios[i] < worst_ratio) {
			worst_ratio = new_distribution->ratios[i];
			worst_processor = i;
		}

		assigned_blocks = round(block_total_count * new_distribution->ratios[i]);
		workload->counts[i] = assigned_blocks * workload->blocksize;
		remaining -= assigned_blocks;

		if (i + 1 < workload->num_procs)
			workload->displs[i + 1] = workload->displs[i] + workload->counts[i];
	}

	if (remaining > 0)
		workload->counts[best_processor] += remaining * workload->blocksize;
	else if (remaining < 0)
		workload->counts[worst_processor] += remaining * workload->blocksize;

	dbglog_info("         new-counts: ");
	int sum_counts = 0;
	for (int i = 0; i < workload->num_procs; i++) {
		dbglog_append("%d ", workload->counts[i]);
		sum_counts += workload->counts[i];
	}
	dbglog_append("(%d == %lu)\n", sum_counts, workload->size);
}


static const class_t _Ullmf_strategy = {
    .size = sizeof(ullmf_strategy_t),
    .name = ullmf_strategy_class,
    .constructor = 0,
    .destructor = 0,
};

const void * Ullmf_strategy = &_Ullmf_strategy;

