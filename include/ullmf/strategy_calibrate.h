/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_strategy_calibrate.h
 * Date: 20/06/2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#ifndef ULLMF_STRATEGY_CALIBRATE_H
#define ULLMF_STRATEGY_CALIBRATE_H

#include "ullmf/class_utils.h"
#include "ullmf/calibration.h"
#include "ullmf/strategy.h"
#include "ullmf/measurement_device.h"

#define ullmf_strategy_calibrate_class "strategy_calibrate"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ullmf_strategy_calibrate ullmf_strategy_calibrate_t;

/** Strategy calibrate.
 *  Redistributes workload based on the processor speed (workload units per second) to minimize
 *  the idle times between problem iterations.
 */
struct ullmf_strategy_calibrate {
    /** Object inheritance */
    ullmf_strategy_t parent;

    /**
     * Normalized difference of time (1 - (worst_time / best_time) < threshold)
     * allowed between all processes to consider them balanced
     */
    double threshold;
};

/**
 * External object instantiation to be used by users.
 */
extern ullmf_strategy_t * ullmf_strategy_calibrate;

#ifdef __cplusplus
}
#endif


#endif
