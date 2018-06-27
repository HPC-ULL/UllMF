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

#include <string.h>
#include <stdarg.h>
#include <assert.h>

#include "ullmf_class_utils.h"
#include "ullmf_measurement_device.h"

enum ullmf_class_error class_typecheck(void* self, const char * classname) {
    class_t self_cl = *((class_t *) self);
    if (strcmp(self_cl.name, classname))
        return ULLMF_CLASS_WRONG_NAME;
    return ULLMF_CLASS_SUCCESS;
}

void* _new(const void* _class, ...) {
    const class_t* true_class = _class;

    void * _object = calloc(1, true_class->size);
    *(class_t const **) _object = true_class;

    va_list args;
    va_start(args, _class);
    _object = true_class->constructor(_object, &args);
    va_end(args);

    return _object;
}

void _delete(void* self) {
	if (self) {
        const class_t** self_class = self;
    	self = (*self_class)->destructor(self);
	}
	free(self);
}
