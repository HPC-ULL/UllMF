/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ulmf_workload.c
 * Date: 20 jun. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#include <stdarg.h>
#include <string.h>

#include "ullmf_workload.h"
#include "ullmf_class_utils.h"

static void set_workload(ullmf_workload_t* self, const int * const counts, const int * const displs) {
	 size_t memsize = sizeof(self->counts) * self->num_procs;
	 memcpy(self->counts, counts, memsize);
	 memcpy(self->displs, displs, memsize);
	 self->size = self->displs[self->num_procs - 1] + self->counts[self->num_procs - 1];
}

static void set_blocksize(ullmf_workload_t* self, const int block_size) {
	self->blocksize = block_size;
}

static void * ullmf_workload_t_constructor(void * self, va_list * args) {
    ullmf_workload_t* _self = self;

    _self->num_procs = va_arg(*args, int);
    int * counts = va_arg(*args, int *);
    int * displs = va_arg(*args, int *);

    size_t memsize = sizeof(_self->counts) * _self->num_procs;
    _self->counts = malloc(memsize);
    memcpy(_self->counts, counts, memsize);

    memsize = sizeof(_self->displs) * _self->num_procs;
    _self->displs = malloc(memsize);
    memcpy(_self->displs, displs, memsize);

    _self->size = _self->displs[_self->num_procs - 1] + _self->counts[_self->num_procs - 1];
    _self->blocksize = va_arg(*args, int);

    // Functions
    _self->set_workload = &set_workload;
    _self->set_blocksize = &set_blocksize;
    return _self;
}

static void * ullmf_workload_t_destructor(void * self) {
    ullmf_workload_t* true_self = self;

    free(true_self->counts);
    true_self->counts = 0;

    free(true_self->displs);
    true_self->displs = 0;

	return true_self;
}


static const class_t _Workload = {
    .size = sizeof(ullmf_workload_t),
    .name = ullmf_workload_class,
    .constructor = &ullmf_workload_t_constructor,
    .destructor = &ullmf_workload_t_destructor,
};

const void* Workload = &_Workload;
