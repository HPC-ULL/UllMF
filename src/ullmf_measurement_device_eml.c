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
#include <eml.h>
#include <eml/data.h>
#include <stdbool.h>
#include <string.h>

static enum ullmf_measurement_error get_eml_measurements(struct measurement_device_eml * md_eml) {
    if (!md_eml->parent.measuring) {
        return ULLMF_MEASUREMENT_NOT_STARTED;
    }

    emlData_t* data[md_eml->ndevices];
    md_eml->err = emlStop(data);
    if (md_eml->err != EML_SUCCESS)
        return ULLMF_MEASUREMENT_INTERNAL_LIBRARY_ERROR;

    md_eml->parent.measurement = -1;

    const char* devname;
    emlDevice_t* dev;
    for (size_t i = 0; i < md_eml->ndevices; i++) {
        double consumed;
        emlDeviceByIndex(i, &dev);
        emlDeviceGetName(dev, &devname);

        if (strstr(devname, ullmf_eml_all_devices) || strstr(devname, md_eml->device)) {
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
    md_eml->parent.measuring = false;

    return ULLMF_MEASUREMENT_SUCCESS;
}

// TODO change to variable number of arguments
static enum ullmf_measurement_error init(void* self) {
    if (class_typecheck(self, ullmf_eml_class))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    struct measurement_device_eml * self_md_eml = (struct measurement_device_eml *) self;

    self_md_eml->err = emlInit();
    if (self_md_eml->err != EML_SUCCESS)
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
    // There is an error in EML that corrupts memory when shutdown is performed
    //self_md_eml->err = emlShutdown();
    //if (self_md_eml->err != EML_SUCCESS)
    //    return ULLMF_MEASUREMENT_INTERNAL_LIBRARY_ERROR;

    self_md_eml->ndevices = -1;
    enum ullmf_measurement_error err = get_eml_measurements(self_md_eml);
    return err;
}

static enum ullmf_measurement_error measurement_start(void* self) {
    if (class_typecheck(self, ullmf_eml_class))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    struct measurement_device_eml * self_md_eml = (struct measurement_device_eml *) self;
    if (self_md_eml->ndevices > 0) {
        self_md_eml->err = emlStart();
        if (self_md_eml->err != EML_SUCCESS)
            return ULLMF_MEASUREMENT_INTERNAL_LIBRARY_ERROR;
        self_md_eml->parent.measuring = true;
    } else {
        return ULLMF_MEASUREMENT_INTERNAL_LIBRARY_ERROR;
    }
    return ULLMF_MEASUREMENT_SUCCESS;
}

static enum ullmf_measurement_error measurement_stop(void* self) {
    if (class_typecheck(self, ullmf_eml_class))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    struct measurement_device_eml * self_md_eml = (struct measurement_device_eml *) self;
    // Checks if measurement_start has been called
    enum ullmf_measurement_error err = get_eml_measurements(self_md_eml);
    return err;
}

measurement_device_eml_t eml_device = {
        .parent._class.name = ullmf_eml_class,
        .parent.measuring = 0,
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
        .ndevices = -1
};

