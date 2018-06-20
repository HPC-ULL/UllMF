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
#ifndef ULLMF_MEASUREMENT_DEVICE_EML_H
#define ULLMF_MEASUREMENT_DEVICE_EML_H

#include "ullmf_measurement_device.h"
#include <eml.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ullmf_eml_class "ullmf_eml_class"
#define ullmf_eml_all_devices "all"

typedef struct measurement_device_eml measurement_device_eml_t;

/** Contains state, properties and methods for a dummy device */
struct measurement_device_eml {
    measurement_device_t parent;

    emlError_t err;

    double time;

    const char * device;

    size_t ndevices;
};

#ifdef __cplusplus
}
#endif

#endif
