/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_distribution.c
 * Date: 20 jun. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#include <stdarg.h>
#include <string.h>

#include "ullmf_distribution.h"
#include "ullmf_class_utils.h"

static void set_ratios(ullmf_distribution_t * self, const int num_procs, const double * const ratios) {
	self->num_procs = num_procs;
	size_t memsize = self->num_procs * sizeof(self->ratios);
	memcpy(self->ratios, ratios, memsize);
    self->total = 0;
	for (int i = 0; i < self->num_procs; i++)
		self->total += ratios[i];
}

static int get_num_procs(ullmf_distribution_t * self) {
	return self->num_procs;
}

static double get_total(ullmf_distribution_t * self) {
	return self->total;
}

static void * ullmf_distribution_t_constructor(void * self, va_list * args) {
	ullmf_distribution_t * _self = self;

    _self->num_procs = va_arg(*args, int);
    double * ratios = va_arg(*args, double *);

	size_t memsize = _self->num_procs * sizeof(_self->ratios);
    _self->ratios = malloc(memsize);
	memcpy(_self->ratios, ratios, memsize);

	_self->total = 0;
    for(int i = 0; i < _self->num_procs; i++)
    	_self->total += _self->ratios[i];

    // Functions
    _self->set_ratios = &set_ratios;
    _self->get_num_procs = &get_num_procs;
    _self->get_total = &get_total;
    return _self;
}

static void * ullmf_distribution_t_destructor(void * self) {
	ullmf_distribution_t* _self = self;

    free(_self->ratios);
    _self->ratios = 0;
    _self->total = 0;
	_self->num_procs = 0;

	return _self;
}


static const class_t _Distribution = {
    .size = sizeof(ullmf_distribution_t),
    .name = ullmf_candidate_class,
    .constructor = &ullmf_distribution_t_constructor,
    .destructor = &ullmf_distribution_t_destructor,
};

const void * Distribution = &_Distribution;
