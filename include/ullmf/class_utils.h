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

/**
 * @file
 * Internal functions to simulate classes and objects in C.
 * @ingroup internalapi
 */


#ifdef __cplusplus
extern "C" {
#endif

/**
 * Error Management for class creation.
 */
enum ullmf_class_error {
    /// Class operation successful.
    ULLMF_CLASS_SUCCESS = 0,
    /// Trying to call a class operation from an object that belongs to a diferent class.
    ULLMF_CLASS_WRONG_NAME
};

/** Struct class_st is used as class_t through the project */
typedef struct class_st class_t;

/**
 * Class defining structure.
 */
struct class_st {
    /// Memory required for the class to be initialized.
    size_t size;
    /// Class name
    const char* name;
    /// Constructor of the class
    void * (* constructor) (void * _class, va_list* args);
    /// Class destructor
    void * (* destructor) (void * _class);
};

/**
 * Checks if an object belongs to a class
 *
 * @param self: object to be checked.
 * @param classname: string with the class name of the instanced object.
 *
 * @retval ULLMF_CLASS_SUCCESS if the object has the correct classname
 */
enum ullmf_class_error class_typecheck(void * self, const char * classname);

/**
 * Creates a class of type
 * @param _class: pointer to a data structure whose first member is struct class_st
 * The _class constructor is called, modifying _class accordingly.
 *
 * @retval Pointer to an object of the specified class
 */
void * _new(const void * _class, ...);

/**
 * Delete for the _class
 * @param _class: pointer to a data structure whose first member is struct class_st
 * The _class destructor is called, modifying _class accordingly.
 */
void _delete(void * _class);


#ifdef __cplusplus
}
#endif


#endif
