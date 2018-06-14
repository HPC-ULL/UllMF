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

enum ullmf_class_error class_typecheck(void* self, const char * classname) {
    struct measurement_device * self_md = (struct measurement_device *) self;
    printf("TYPECHECK %s", self_md->_class);
    if (strcmp(self_md->_class, classname))
        return ULLMF_CLASS_WRONG_CLASS;
    return ULLMF_CLASS_SUCCESS;
}
