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

#include "ullmf_strategy.h"
#include "ullmf_strategy_calibrate.h"
#include "ullmf_measurement_device.h"
#include "ullmf_measurement_device_mpi.h"
#include "debug.h"

#include <float.h>
#include <ullmf_distribution.h>

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
}

static double * get_current_ratios(ullmf_calibration_t* calib) {
	double * to_return = calloc(calib->num_procs, sizeof(double));
	dbglog_info("        time ratios: ");
	double time_ratio_sum = 0;
	for (int i = 0; i < calib->num_procs; i++) {
		if (calib->measurements[i] == 0)
			to_return[i] = 0;
		else
			to_return[i] = calib->workload->counts[i] / calib->measurements[i];
		dbglog_append(DBG_FMT " ", to_return[i]);
		time_ratio_sum += to_return[i];
	}
	dbglog_append("\n");
	if (time_ratio_sum != 1) {
	    for (int i = 0; i < calib->num_procs; i++) {
	    	to_return[i] = to_return[i] / time_ratio_sum;
	    }
	}
	return to_return;
}

static int calibrate(ullmf_calibration_t* calib) {
	ullmf_strategy_calibrate_t* strategy = (ullmf_strategy_calibrate_t*) calib->strategy;

	dbglog_info(" -- Current strategy: (%d) ", strategy->parent._class.name);
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

	ullmf_distribution_t * ratios = get_current_ratios(calib);

//	double ratio_sum = 0;
//    for (int i = 0; i < calib->num_procs; i++) {
//        ratios[i] = ratios[i] / time_ratio_sum;
//        ratio_sum += ratios[i];
//    }

	free(ratios);
    return ULLMF_TAG_RECALIBRATING;
}

ullmf_strategy_calibrate_t ullmf_strategy_calibrate = {
    .parent._class.size = sizeof(ullmf_strategy_calibrate_t),
    .parent._class.name = ullmf_strategy_calibrate_class,
    .parent.mdevice = (measurement_device_t *) &mpi_device,
    .parent.calibrate = &calibrate,
    .parent.redistribute = &ullmf_strategy_redistribute,
	.threshold = 0.05,
};
