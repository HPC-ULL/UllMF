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

#ifndef _INCLUDE_ULLMF_UTILS_H_
#define _INCLUDE_ULLMF_UTILS_H_

/**
 * @file
 * Internal generic functions required by various modules.
 * @ingroup internalapi
 */

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Is a double zero?.
 * @param value: Value to be compared to zero.
 * @param tolerance: Limit for the double value to be considered 0.
 *
 * @return: True if | value | < tolerance
 */
bool f_is_zero(const double value, const double tolerance);

#ifdef __cplusplus
}
#endif

#endif /* _INCLUDE_ULLMF_UTILS_H_ */
