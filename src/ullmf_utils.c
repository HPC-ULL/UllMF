/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_utils.c
 * Date: 11 Oct. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#include "ullmf/debug.h"
#include "ullmf/utils.h"

#include <stdbool.h>

bool f_is_zero(const double remaining_movement, const double zero_tolerance) {
//	dbglog_info("  is_zero? %.2f, %d\n", remaining_movement,
//			remaining_movement > -zero_tolerance && remaining_movement < zero_tolerance);
	return remaining_movement > -zero_tolerance && remaining_movement < zero_tolerance;
}
