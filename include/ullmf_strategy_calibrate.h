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

#include "ullmf_class_utils.h"
#include "ullmf_calibration.h"
#include "ullmf_strategy.h"
#include "ullmf_measurement_device.h"

#define ullmf_strategy_calibrate_class "strategy_calibrate"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ullmf_strategy_calibrate ullmf_strategy_calibrate_t;

/** Contains state, properties and methods for an strategy */
struct ullmf_strategy_calibrate {
    ullmf_strategy_t parent;

    double threshold;
};

extern ullmf_strategy_t * ullmf_strategy_calibrate;

#ifdef __cplusplus
}
#endif


#endif
