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
#include "ullmf_class_utils.h"
#include <string.h>

#include <stdio.h>


// TODO change to variable number of arguments
static enum ullmf_measurement_error init(void* self, int id) {
    if (class_typecheck(self, dummy_class))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    struct measurement_device_dummy * self_md_dummy = (struct measurement_device_dummy *) self;
    self_md_dummy->id = id;
    printf("Self ID %d", self_md_dummy->id);
    return ULLMF_MEASUREMENT_SUCCESS;
}

static enum ullmf_measurement_error shutdown(void* self) {
    if (class_typecheck(self, dummy_class))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    return ULLMF_MEASUREMENT_SUCCESS;
}

static enum ullmf_measurement_error measurement_start(void* self) {
    // Do nothing
    if (class_typecheck(self, dummy_class))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    struct measurement_device_dummy * self_md_dummy = (struct measurement_device_dummy *) self;
    self_md_dummy->measurement_ll = millitimestamp();
    return ULLMF_MEASUREMENT_SUCCESS;
}

static enum ullmf_measurement_error measurement_stop(void* self) {
    // Do nothing
    if (class_typecheck(self, dummy_class))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    struct measurement_device_dummy * self_md_dummy = (struct measurement_device_dummy *) self;
    self_md_dummy->measurement_ll = millitimestamp() - self_md_dummy->measurement_ll;
    self_md_dummy->parent.measurement = (double) self_md_dummy->measurement_ll;
    return ULLMF_MEASUREMENT_SUCCESS;
}

struct measurement_device_dummy dummy_device = {
        .parent._class = dummy_class,
        .parent.initialized = 0,
        .parent.measurement = .0,
        .parent.unit = "None",
        .parent.init = &init,
        .parent.shutdown = &shutdown,
        .parent.measurement_start = &measurement_start,
        .parent.measurement_stop = &measurement_stop,
        .measurement_ll = 0,
        .id = 0,
};

