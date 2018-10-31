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

#include "ullmf_measurement_device.h"
#include "ullmf_measurement_device_eml.h"
#include "ullmf_class_utils.h"
#include "ullmf_timer.h"
#include "debug.h"
#include <mpi.h>
#include <eml.h>
#include <eml/data.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define INTERNAL_CALIBRATION_INTERVAL 10
#define MEASUREMENT_TIME_INTERVAL 200 // 200 ms
#define INTERNAL_REFRESH_INTERVAL 5

static enum ullmf_measurement_error get_eml_measurements(struct measurement_device_eml * md_eml) {
    emlData_t* data[md_eml->ndevices];
    md_eml->err = emlStop(data);
    if (md_eml->err != EML_SUCCESS)
        return ULLMF_MEASUREMENT_INTERNAL_LIBRARY_ERROR;

    md_eml->parent.measurement = 0;

    const char* devname;
    emlDevice_t* dev;
    for (size_t i = 0; i < md_eml->ndevices; i++) {
        double consumed;
        emlDeviceByIndex(i, &dev);
        emlDeviceGetName(dev, &devname);
        if (strstr(md_eml->device, ullmf_eml_all_devices) || strstr(devname, md_eml->device)) {
            md_eml->err = emlDataGetConsumed(data[i], &consumed);

            if (md_eml->err != EML_SUCCESS)
                return ULLMF_MEASUREMENT_INTERNAL_LIBRARY_ERROR;

            md_eml->err = emlDataGetElapsed(data[i], &md_eml->time);
            if (md_eml->err != EML_SUCCESS)
                return ULLMF_MEASUREMENT_INTERNAL_LIBRARY_ERROR;


            md_eml->parent.measurement += consumed;
        }
    }

    md_eml->err = emlDataFree(*data);
    if (md_eml->err != EML_SUCCESS)
        return ULLMF_MEASUREMENT_INTERNAL_LIBRARY_ERROR;
    return ULLMF_MEASUREMENT_SUCCESS;
}

// TODO change to variable number of arguments
static enum ullmf_measurement_error init(void* self) {
    if (class_typecheck(self, ullmf_eml_class))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    struct measurement_device_eml * self_md_eml = (struct measurement_device_eml *) self;

    self_md_eml->err = emlInit();
    if (self_md_eml->err != EML_SUCCESS && self_md_eml->err != EML_ALREADY_INITIALIZED)
        return ULLMF_MEASUREMENT_INTERNAL_LIBRARY_ERROR;
    self_md_eml->err = emlDeviceGetCount(&self_md_eml->ndevices);
    if (self_md_eml->err != EML_SUCCESS)
        return ULLMF_MEASUREMENT_INTERNAL_LIBRARY_ERROR;

    return ULLMF_MEASUREMENT_SUCCESS;
}

static enum ullmf_measurement_error shutdown(void* self) {
    if (class_typecheck(self, ullmf_eml_class))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    struct measurement_device_eml * self_md_eml = (struct measurement_device_eml *) self;
    if (self_md_eml->parent.measuring)
    	self_md_eml->parent.measurement_stop(self);
    // TODO There is an error in EML that corrupts memory when shutdown is performed
    self_md_eml->err = emlShutdown();
    if (self_md_eml->err != EML_SUCCESS)
        return ULLMF_MEASUREMENT_INTERNAL_LIBRARY_ERROR;

    self_md_eml->ndevices = -1;
    return ULLMF_MEASUREMENT_SUCCESS;
}

static enum ullmf_measurement_error measurement_start(void* self) {
    if (class_typecheck(self, ullmf_eml_class))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    struct measurement_device_eml * self_md_eml = (struct measurement_device_eml *) self;
    if (self_md_eml->parent.measuring) {
        return ULLMF_MEASUREMENT_STARTED;
    }
    if (self_md_eml->ndevices > 0) {
        if (self_md_eml->measurement_interval != 0 &&
        		self_md_eml->current_it == 0) {
            self_md_eml->err = emlStart();
            if (self_md_eml->err != EML_SUCCESS)
                return ULLMF_MEASUREMENT_INTERNAL_LIBRARY_ERROR;
            self_md_eml->parent.measuring = true;
        } else {
            if (!self_md_eml->interval_calc_started) {
            	self_md_eml->interval_calc_started = true;
            	self_md_eml->first_calibration_t = millitimestamp();
            }
            self_md_eml->parent.measuring = true;
        }
    	self_md_eml->current_it++;
    } else {
    	if (self_md_eml->ndevices == 0) {
    		dbglog_warn("No EML measurement devices found.\n");
    	}
    	return ULLMF_MEASUREMENT_INTERNAL_LIBRARY_ERROR;
    }
    return ULLMF_MEASUREMENT_SUCCESS;
}

static enum ullmf_measurement_error calculate_measurement_interval(measurement_device_eml_t* self) {
	self->first_calibration_t = millitimestamp() - self->first_calibration_t;
	unsigned long long iteration_time = self->first_calibration_t / self->internal_calibration_interval;
	self->measurement_interval = self->measurement_time_interval / iteration_time;

	if (!self->measurement_interval)
		self->measurement_interval = 1;
	MPI_Allreduce(&self->measurement_interval, &self->measurement_interval, 1, MPI_INT,
			MPI_MAX, self->eml_comm);
	return ULLMF_MEASUREMENT_SUCCESS;
}

static enum ullmf_measurement_error measurement_stop(void* self) {
    if (class_typecheck(self, ullmf_eml_class))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    measurement_device_eml_t* self_md_eml = (measurement_device_eml_t *) self;
    enum ullmf_measurement_error err = ULLMF_MEASUREMENT_SUCCESS;
    if (!self_md_eml->parent.measuring) {
    	return ULLMF_MEASUREMENT_NOT_STARTED;
    }
	self_md_eml->parent.measuring = false;
    // self_md_eml->current_it always >= 0 && self_md_eml->measurement_interval == 0 only on startup
    if (self_md_eml->measurement_interval == self_md_eml->current_it) {
    	err = get_eml_measurements(self_md_eml);
    	if (err != ULLMF_MEASUREMENT_SUCCESS)
    		return err;
    	self_md_eml->current_it = 0;
    	// TODO Measurement refresh
    	/*
    	 * calib->energy->last_refresh--;
            if (!calib->energy->last_refresh) {
                calib->energy->iteration_interval =
                floor(calib->energy->measurement_interval / (elapsedt / calib->energy->iteration_interval));
                MPI_Allreduce(&calib->energy->iteration_interval, &calib->energy->iteration_interval,
                1, MPI_INT, MPI_MAX, calib->comm);
                if (calib->comm_root == calib->comm_rank)
                dbglog_info("Calibrating every %zu iterations. CURR_IT: %zu, INTERVAL: %d\n",
                        calib->energy->iteration_interval, calib->current_iteration, INTERVAL_ITERATIONS);
            }
    	 */
    } else {
    	if (self_md_eml->current_it % self_md_eml->internal_calibration_interval == 0) {
    		err = calculate_measurement_interval(self_md_eml);
        	self_md_eml->current_it = 0;
    	}
    }

    return err;
}

measurement_device_eml_t ullmf_eml_device = {
        .parent._class.name = ullmf_eml_class,
        .parent.measuring = false,
        .parent.measurement = .0,
        .parent.unit = "None",
        .parent.init = &init,
        .parent.shutdown = &shutdown,
        .parent.measurement_start = &measurement_start,
        .parent.measurement_stop = &measurement_stop,
        .parent.get_measurement = &measurement_device_get_measurement,
        .err = EML_SUCCESS,
        .time = 0,
        .device = ullmf_eml_all_devices,
        .ndevices = -1,
		.interval_calc_started = false,
		.measurement_interval = 0,
		.measurement_time_interval = MEASUREMENT_TIME_INTERVAL,
		.current_it = 0,
        .first_calibration_t = 0,
		.internal_calibration_interval = INTERNAL_CALIBRATION_INTERVAL,
		.eml_comm = MPI_COMM_WORLD,
};

