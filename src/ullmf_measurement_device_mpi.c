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
#include "ullmf_measurement_device_mpi.h"
#include <mpi.h>
#include <stdbool.h>
#include "ullmf_class_utils.h"

// TODO change to variable number of arguments
static enum ullmf_measurement_error init(void* self) {
    if (class_typecheck(self, ullmf_mpi_class))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    return ULLMF_MEASUREMENT_SUCCESS;
}

static enum ullmf_measurement_error shutdown(void* self) {
    if (class_typecheck(self, ullmf_mpi_class))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    return ULLMF_MEASUREMENT_SUCCESS;
}

static enum ullmf_measurement_error measurement_start(void* self) {
    // Do nothing
    if (class_typecheck(self, ullmf_mpi_class))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    struct measurement_device_mpi * self_md_mpi = (struct measurement_device_mpi *) self;
    if (self_md_mpi->parent.measuring) {
        return ULLMF_MEASUREMENT_STARTED;
    } else {
        self_md_mpi->parent.measurement = MPI_Wtime();
        self_md_mpi->parent.measuring = true;
    }
    return ULLMF_MEASUREMENT_SUCCESS;
}

static enum ullmf_measurement_error measurement_stop(void* self) {
    // Do nothing
    if (class_typecheck(self, ullmf_mpi_class))
        return ULLMF_MEASUREMENT_WRONG_CLASS;
    struct measurement_device_mpi * self_md_mpi = (struct measurement_device_mpi *) self;
    if (self_md_mpi->parent.measuring) {
        self_md_mpi->parent.measurement = MPI_Wtime() - self_md_mpi->parent.measurement;
        self_md_mpi->parent.measuring = false;
    } else {
        return ULLMF_MEASUREMENT_NOT_STARTED;
    }
    return ULLMF_MEASUREMENT_SUCCESS;
}

measurement_device_mpi_t ullmf_mpi_device = {
        .parent._class.name = ullmf_mpi_class,
        .parent.measuring = 0,
        .parent.measurement = .0,
        .parent.unit = "None",
        .parent.init = &init,
        .parent.shutdown = &shutdown,
        .parent.measurement_start = &measurement_start,
        .parent.measurement_stop = &measurement_stop,
        .parent.get_measurement = &measurement_device_get_measurement,
};

