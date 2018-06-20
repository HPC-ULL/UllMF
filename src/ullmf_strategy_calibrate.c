/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_strategy_calibrate.c
 * Date: 20 jun. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#include "ullmf_strategy_calibrate.h"
#include "ullmf_measurement_device.h"
#include "ullmf_measurement_device_mpi.h"

extern measurement_device_mpi_t mpi_device;

static void calibrate(struct ullmf_calibration* calib) {

}

ullmf_strategy_calibrate_t strategy_calibrate = {
    .parent._class = ullmf_strategy_calibrate_class,
    .parent.mdevice = (measurement_device_t *) &mpi_device,
    .parent.calibrate = &calibrate,
    .parent.redistribute = &ullmf_strategy_redistribute,
};
