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

#ifndef ULLMF_CLASS_H
#define ULLMF_CLASS_H

#include <stdlib.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

enum ullmf_class_error {
    ULLMF_CLASS_SUCCESS = 0,
    ULLMF_CLASS_WRONG_NAME
};

typedef struct class_st class_t;

struct class_st {
    size_t size;
    const char* name;
    void * (* constructor) (void * _class, va_list* args);
    void * (* destructor) (void * _class);
};

enum ullmf_class_error class_typecheck(void * self, const char * classname);

void * _new(const void * _class, ...);
void * _delete(void * _class);


#ifdef __cplusplus
}
#endif


#endif
