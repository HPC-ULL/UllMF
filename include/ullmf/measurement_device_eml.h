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

#include "ullmf/measurement_device.h"
#include <stdbool.h>
#include <eml.h>
#include <mpi.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ullmf_eml_class "ullmf_eml_class"
#define ullmf_eml_all_devices "all"

typedef struct measurement_device_eml measurement_device_eml_t;

/** Measurement device that uses EML to provide energy measurements */
struct measurement_device_eml {
    /**
     * Object inheritance
     */
    measurement_device_t parent;

    /**
     * Error handling for EML.
     */
    emlError_t err;

    /**
     * Time spent since last measurement
     */
    double time;

    /**
     * Last read EML device name
     */
    const char * device;

    /**
     * Number of Available EML devices
     */
    size_t ndevices;

    /**
     * Number of Available EML devices
     */
    bool interval_calc_started;

    /**
     * Internal state for managing internal_calibration_interval
     */
    bool measuring;

    /**
     * Number of iterations between measurements.
     * This is used to synchronize the distributed EML measurements in different processes.
     */
    size_t measurement_interval;

    /**
     * Estimated amount of time in between measurements. Used to adjust the library to the actual physical device being used to measure energy.
     * Defaults to 150ms, but have to be adjusted to the physical device in order to improve the dynamic load balancing procedures.
     */
    unsigned long long measurement_time_interval;

    /**
     * Next iteration that will trigger a new measurement
     */
    unsigned long long next_start;

    /**
     * Next iteration that will stop a measurement
     */
    unsigned long long next_stop;

    /**
     * Current internal iteration in the measurement device. Increases with every measurement_stop call.
     */
    size_t current_it;

    /**
     * Time in milliseconds that is equivalent to measurement_time_interval number of iterations.
     */
    unsigned long long first_calibration_t;

    /**
     * Number of iterations in the problem that have to pass to estimate measurement_interval.
     */
    size_t internal_calibration_interval;

    /** MPI Communicator for internal use in this measurement device */
    MPI_Comm eml_comm;
};

/** Initialization of the object for usage inside the strategies. */
extern measurement_device_eml_t ullmf_eml_device;

#ifdef __cplusplus
}
#endif

#endif
