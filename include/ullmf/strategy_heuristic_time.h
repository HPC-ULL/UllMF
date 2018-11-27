/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_strategy_heuristic_time.h
 * Date: 20 jun. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#ifndef ULLMF_STRATEGY_HEURISTIC_TIME_H
#define ULLMF_STRATEGY_HEURISTIC_TIME_H

#include "ullmf/strategy.h"
#include "ullmf/strategy_heuristic.h"


#ifdef __cplusplus
extern "C" {
#endif

#define ullmf_strategy_heuristic_time_class "ullmf_strategy_heuristic_time"

typedef struct ullmf_strategy_heuristic_time ullmf_strategy_heuristic_time_t;

/** Contains state, properties and methods for an strategy */
struct ullmf_strategy_heuristic_time {
    ullmf_strategy_heuristic_t parent;
};

extern ullmf_strategy_t * ullmf_strategy_heuristic_time;


#ifdef __cplusplus
}
#endif


#endif
