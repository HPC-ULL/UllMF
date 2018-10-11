/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_strategy_calibrate.c
 * Date: 20 jun. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#include "ullmf_distribution.h"
#include "ullmf_strategy.h"
#include "ullmf_strategy_calibrate.h"
#include "ullmf_measurement_device.h"
#include "ullmf_measurement_device_mpi.h"
#include "debug.h"

#include <float.h>
#include <ullmf_distribution.h>

static const double epsilon = 0.0005;

static void calculate_totals(ullmf_calibration_t* calib, double * total_time, double * tmax, double * tmin) {
	*total_time = 0;
	*tmax = 0;
	*tmin = DBL_MAX;
	dbglog_info("          time spent: ");
	for (int i = 0; i < calib->num_procs; i++) {
		dbglog_append("%.4f ", calib->measurements[i]);
		*total_time += calib->measurements[i];
		if (calib->measurements[i] == 0)
			continue;
		if (calib->measurements[i] > *tmax)
			*tmax = calib->measurements[i];
		if (calib->measurements[i] < *tmin)
			*tmin = calib->measurements[i];
	}
	dbglog_append("\n");
}

static ullmf_distribution_t * get_current_performance(ullmf_calibration_t* calib) {
	double * ratios = calloc(calib->num_procs, sizeof(double));
	dbglog_info("        time ratios: ");
	double total_speed = 0;

	for (int i = 0; i < calib->num_procs; i++) {
		if (calib->measurements[i] > -epsilon && calib->measurements[i] < epsilon) // == 0
			ratios[i] = 0;
		else
			ratios[i] = calib->workload->counts[i] / calib->measurements[i];
		dbglog_append(DBG_FMT " ", ratios[i]);
		total_speed += ratios[i];
	}

	dbglog_append("\n");
	for (int i = 0; i < calib->num_procs; i++) {
		ratios[i] = ratios[i] / total_speed;
	}

	ullmf_distribution_t * to_ret = _new(Distribution, calib->num_procs, ratios);
	free(ratios);
	return to_ret;
}

static int calibrate(ullmf_calibration_t* calib) {
	ullmf_strategy_calibrate_t* strategy = (ullmf_strategy_calibrate_t*) calib->strategy;
	dbglog_info(" -- Current strategy: (%s) \n", strategy->parent._class.name);
    double total_time, tmax, tmin;
    calculate_totals(calib, &total_time, &tmax, &tmin);

	if ((tmax == DBL_MIN) || (tmin == DBL_MAX) ||
		((1 - tmin / tmax) < strategy->threshold))
		return ULLMF_TAG_CALIBRATED;

	dbglog_info("             counts: ");
	for (int i = 0; i < calib->num_procs; i++) {
		dbglog_append("%d ", calib->workload->counts[i]);
	}
	dbglog_append("(%zu)\n", calib->workload->size);

	if (calib->strategy->best_candidate != 0)
		_delete(calib->strategy->best_candidate);
	calib->strategy->best_candidate = get_current_performance(calib);

    return ULLMF_TAG_RECALIBRATING;
}

static ullmf_strategy_calibrate_t _ullmf_strategy_calibrate = {
    .parent._class.size = sizeof(ullmf_strategy_calibrate_t),
    .parent._class.name = ullmf_strategy_calibrate_class,
    .parent.mdevice = (measurement_device_t *) &ullmf_mpi_device,
    .parent.calibrate = &calibrate,
    .parent.redistribute = &ullmf_strategy_redistribute,
	.parent.best_candidate = 0,
	.threshold = 0.05,
};

ullmf_strategy_t * ullmf_strategy_calibrate = (ullmf_strategy_t *) &_ullmf_strategy_calibrate;
