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

#ifdef __cplusplus
extern "C" {
#endif

enum ullmf_measurement_error {
    ULLMF_MEASUREMENT_SUCCESS = 0,
    ULLMF_MEASUREMENT_STARTED,
    ULLMF_MEASUREMENT_NOT_STARTED,
    ULLMF_MEASUREMENT_WRONG_CLASS
};

/** Contains state, properties and methods for a device type */
struct measurement_device {
    /** Device Name */
    const char* _class;

    /** Whether the device is initialized.
     *
     * 1 if it is initialized, 0 if it is not.
     */
    int initialized;

    /** Last measurement taken.
     */
    double measurement;

    /** Measurement unit.
     *
     */
    const char* unit;

    // TODO Change to variable number of arguments to allow initialization of children
    /**
     * Initializes the measurement_device.
     *
     * @param[in] self the measurement device itself
     *
     * @retval ULLMF_MEASUREMENT_SUCCESS The measurement was initialized
     */
    enum ullmf_measurement_error (*init)(void* self, int id);

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
};

#ifdef __cplusplus
}
#endif

#endif
