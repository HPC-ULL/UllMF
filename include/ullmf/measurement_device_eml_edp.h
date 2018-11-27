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
#ifndef ULLMF_MEASUREMENT_DEVICE_EML_EDP_H
#define ULLMF_MEASUREMENT_DEVICE_EML_EDP_H

#include "ullmf/measurement_device.h"
#include <stdbool.h>
#include <eml.h>
#include <mpi.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ullmf_eml_class "ullmf_eml_edp_class"
#define ullmf_eml_all_devices "all"

typedef struct measurement_device_eml_edp measurement_device_eml_edp_t;

/** Contains state, properties and methods for a dummy device */
struct measurement_device_eml_edp {
    measurement_device_t parent;

    emlError_t err;

    double time;

    const char * device;

    size_t ndevices;

    bool interval_calc_started;

    bool measuring; // Internal state to manage internal_calibration_interval

    size_t measurement_interval;

    unsigned long long measurement_time_interval;

    unsigned long long next_start;

    unsigned long long next_stop;

    size_t current_it;

    unsigned long long first_calibration_t;

    size_t internal_calibration_interval;

    MPI_Comm eml_comm;
};

extern measurement_device_eml_edp_t ullmf_eml_edp_device;

#ifdef __cplusplus
}
#endif

#endif
