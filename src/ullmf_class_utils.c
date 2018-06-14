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

#include "ullmf_class_utils.h"
#include "ullmf_measurement_device.h"
#include <string.h>



enum ullmf_class_error class_typecheck(void* self, const char * classname) {
    struct class_t self_cl = *((struct class_t *) self);
    if (strcmp(self_cl.name, classname))
        return ULLMF_CLASS_WRONG_NAME;
    return ULLMF_CLASS_SUCCESS;
}
