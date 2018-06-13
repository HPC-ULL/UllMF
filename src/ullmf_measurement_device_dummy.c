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
#include "ullmf_measurement_device_dummy.h"
#include "timer.h"
#include "string.h"

static enum ullmf_measurement_error typecheck(void* self) {
    measurement_device_t * self_md = (measurement_device_t *) self;
    if (strcmp(self_md->_class, dummy_class))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    return ULLMF_MEASUREMENT_SUCCESS;
}

// TODO change to variable number of arguments
static enum ullmf_measurement_error init(void* self, int id) {
    if (typecheck(self))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    measurement_device_dummy_t * self_md_dummy = (measurement_device_dummy_t *) self;
    self_md_dummy->id = id;
    return ULLMF_MEASUREMENT_SUCCESS;
}

static enum ullmf_measurement_error shutdown(void* self) {
    if (typecheck(self))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    return ULLMF_MEASUREMENT_SUCCESS;
}

static enum ullmf_measurement_error measurement_start(void* self) {
    // Do nothing
    if (typecheck(self))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    measurement_device_dummy_t * self_md_dummy = (measurement_device_dummy_t *) self;
    self_md_dummy->measurement_ll = millitimestamp();
    return ULLMF_MEASUREMENT_SUCCESS;
}

static enum ullmf_measurement_error measurement_stop(void* self) {
    // Do nothing
    if (typecheck(self))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    measurement_device_dummy_t * self_md_dummy = (measurement_device_dummy_t *) self;
    self_md_dummy->measurement_ll = millitimestamp() - self_md_dummy->measurement_ll;
    self_md_dummy->mdevice->measurement = (double) self_md_dummy->measurement_ll;
    return ULLMF_MEASUREMENT_SUCCESS;
}

measurement_device_t dummy_parent = {
        ._class = dummy_class,
        .initialized = 0,
        .measurement = .0,
        .unit = "None",
        .init = &init,
        .shutdown = &shutdown,
        .measurement_start = &measurement_start,
        .measurement_stop = &measurement_stop,
};

measurement_device_dummy_t dummy_device = {
        .mdevice = &dummy_parent,
        .measurement_ll = 0,
        .id = 0,
};

