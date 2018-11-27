/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 * File: ullmf_calibration.c
 * Date: 31 oct. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#include "ullmf/calibration.h"

static const class_t _Ullmf_calibration = {
    .size = sizeof(ullmf_calibration_t),
    .name = ullmf_calibration_class,
    .constructor = 0,
    .destructor = 0,
};

const void * Ullmf_calibration = &_Ullmf_calibration;
