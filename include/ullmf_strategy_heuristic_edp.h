/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_strategy_heuristic_edp.h
 * Date: 22 Nov. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#ifndef ULLMF_STRATEGY_HEURISTIC_EDP_H
#define ULLMF_STRATEGY_HEURISTIC_EDP_H

#include "ullmf_strategy.h"
#include "ullmf_strategy_heuristic.h"


#ifdef __cplusplus
extern "C" {
#endif
// TODO Once the constructor for the strategy is done, simplify the static strategy modules
// generating strategies with default parameters using that constructor.

#define ullmf_strategy_heuristic_edp_class "ullmf_strategy_heuristic_edp"

typedef struct ullmf_strategy_heuristic_edp ullmf_strategy_heuristic_edp_t;

/** Contains state, properties and methods for an strategy */
struct ullmf_strategy_heuristic_edp {
    ullmf_strategy_heuristic_t parent;
};

extern ullmf_strategy_t * ullmf_strategy_heuristic_edp;

#ifdef __cplusplus
}
#endif


#endif
