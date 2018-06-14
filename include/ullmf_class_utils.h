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

#ifndef ULLMF_CLASS_UTILS_H
#define ULLMF_CLASS_UTILS_H

#include "ullmf_class_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

enum ullmf_class_error {
    ULLMF_CLASS_SUCCESS = 0,
    ULLMF_CLASS_WRONG_CLASS
};

enum ullmf_class_error class_typecheck(void* self, const char * classname);

#ifdef __cplusplus
}
#endif


#endif
