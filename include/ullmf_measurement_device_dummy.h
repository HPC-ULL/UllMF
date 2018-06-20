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
#ifndef ULLMF_MEASUREMENT_DEVICE_DUMMY_H
#define ULLMF_MEASUREMENT_DEVICE_DUMMY_H

#include "ullmf_measurement_device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ullmf_dummy_class "dummy"

typedef struct measurement_device_dummy measurement_device_dummy_t;

/** Contains state, properties and methods for a dummy device */
struct measurement_device_dummy {
    measurement_device_t parent;

    /** Measurement device specific measurement units
     *
     */
    long long measurement_ll;

    int id;
};

#ifdef __cplusplus
}
#endif

#endif
