/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */
#ifndef ULLMF_MEASUREMENT_DEVICE_H
#define ULLMF_MEASUREMENT_DEVICE_H

#include <stdlib.h>
#include <stdbool.h>
#include "ullmf_class_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

enum ullmf_measurement_error {
    ULLMF_MEASUREMENT_SUCCESS = 0,
    ULLMF_MEASUREMENT_STARTED,
    ULLMF_MEASUREMENT_NOT_STARTED,
    ULLMF_MEASUREMENT_WRONG_CLASS,
    ULLMF_MEASUREMENT_INTERNAL_LIBRARY_ERROR,
};

typedef struct measurement_device measurement_device_t;

/** Contains state, properties and methods for a device type */
struct measurement_device {
    /** Object inheritance */
    class_t _class;

    /** Whether the device is measuring.
     *
     * 1 if it is measuring, 0 if it is not.
     */
    bool measuring;

    /** Last measurement taken.
     */
    double measurement;

    /** Measurement unit.
     *
     */
    const char* unit;

    // TODO Change to variable number of arguments to allow initialization of children
    // It is not needed right now with the implemented measurement devices
    /**
     * Initializes the measurement_device.
     *
     * @param[in] self the measurement device itself
     *
     * @retval ULLMF_MEASUREMENT_SUCCESS The measurement was initialized
     */
    enum ullmf_measurement_error (*init)(void* self);

    /**
     * Shuts down the measurement_device\
   *
     * @param[in] self the measurement device itself
     *
     * @retval ULLMF_MEASUREMENT_SUCCESS The measurement was shutdown
     */
    enum ullmf_measurement_error (*shutdown)(void* self);

    /**
     * Takes a measurement from a single device
     *
     * @param[in] self the measurement device itself
     *
     * @retval ULLMF_MEASUREMENT_SUCCESS The measurement was started
     */
    enum ullmf_measurement_error (*measurement_start)(void* self);

    /**
     * Takes a measurement from a single device
     *
     * @param[in] self the measurement device itself
     *
     * @retval ULLMF_MEASUREMENT_SUCCESS The measurement was stopped
     */
    enum ullmf_measurement_error (*measurement_stop)(void* self);

    /**
     * Returns the last measurement taken
     *
     * @retval double_value The last measurement taken
     */
    double (*get_measurement) (void* self);
};

double measurement_device_get_measurement(void* self);

#ifdef __cplusplus
}
#endif

#endif
