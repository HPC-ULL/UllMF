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
#ifndef ULLMF_MEASUREMENT_DEVICE_MPI_H
#define ULLMF_MEASUREMENT_DEVICE_MPI_H

#include "ullmf_measurement_device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ullmf_mpi_class "ullmf_mpi_class"

/** Contains state, properties and methods for a dummy device */
struct measurement_device_mpi {
    struct measurement_device parent;
};

#ifdef __cplusplus
}
#endif

#endif
