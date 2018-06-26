/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_strategy.c
 * Date: 20 jun. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#include "ullmf_strategy.h"
#include "ullmf_calibration.h"

void ullmf_strategy_redistribute(ullmf_calibration_t* calib) {

}


static const class_t _Ullmf_strategy = {
    .size = sizeof(ullmf_strategy_t),
    .name = ullmf_strategy_class,
    .constructor = 0,
    .destructor = 0,
};

const void * Ullmf_strategy = &_Ullmf_strategy;

