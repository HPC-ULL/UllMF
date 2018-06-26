/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_strategy.h
 * Date: 20 jun. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#ifndef ULLMF_STRATEGY_H
#define ULLMF_STRATEGY_H

typedef struct ullmf_strategy ullmf_strategy_t;

#include "ullmf_class_utils.h"
#include "ullmf_calibration.h"
#include "ullmf_measurement_device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ullmf_strategy_class "ullmf_strategy"

enum ullmf_tag {
	ULLMF_TAG_CALIBRATED = 0,
	ULLMF_TAG_RECALIBRATING
};

typedef struct ullmf_strategy ullmf_strategy_t;

/** Contains state, properties and methods for an strategy */
struct ullmf_strategy {
    /** Object inheritance */
    class_t _class;

    /** Measurement device used by the strategy */
    measurement_device_t* mdevice;

    /** Main Calibration Procedure */
    int (*calibrate)(ullmf_calibration_t* calib);

    /** Redistribution procedure */
    void (*redistribute)(ullmf_calibration_t* calib);
};

void ullmf_strategy_redistribute(ullmf_calibration_t* calib);

extern const void * Ullmf_strategy;

#ifdef __cplusplus
}
#endif


#endif
