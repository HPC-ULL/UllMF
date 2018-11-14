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
#include <stdio.h>

void ullmf_strategy_redistribute(ullmf_calibration_t* calib) {
	//dbglog_info("[id = %d] ullmf_strategy_redistribute\n", calib->id);

	ullmf_distribution_t * new_distribution = calib->strategy->best_candidate;
	ullmf_workload_t * old_workload = calib->workload;

    dbglog_info(" redistribute new distr: ");
    for (int i = 0; i < calib->num_procs; i++) {
        dbglog_append("  %.4f", new_distribution->proportional_workload[i]);
    }
    dbglog_append("\n");
	ullmf_workload_t * new_workload = calib->workload->new_from_distribution(calib->workload, new_distribution);
	calib->workload = new_workload;
	_delete(old_workload);

	dbglog_info("         new-counts: ");
	int sum_counts = 0;
	for (int i = 0; i < calib->workload->num_procs; i++) {
		dbglog_append("%d ", calib->workload->counts[i]);
		sum_counts += calib->workload->counts[i];
	}
	dbglog_append("(%d == %lu)\n", sum_counts, calib->workload->size);
}


static const class_t _Ullmf_strategy = {
    .size = sizeof(ullmf_strategy_t),
    .name = ullmf_strategy_class,
    .constructor = 0,
    .destructor = 0,
};

const void * Ullmf_strategy = &_Ullmf_strategy;

