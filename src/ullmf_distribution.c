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
#include <math.h>

#include "debug.h"
#include "ullmf_distribution.h"
#include "ullmf_class_utils.h"

static void set_ratios(ullmf_distribution_t * self, const int num_procs, const double * const ratios) {
	self->num_procs = num_procs;
	size_t memsize = self->num_procs * sizeof(self->ratios);
	memcpy(self->ratios, ratios, memsize);
    self->total = 0;
	for (int i = 0; i < self->num_procs; i++)
		self->total += ratios[i];
	self->redistribute_remainder(self);
}

static int get_num_procs(ullmf_distribution_t * self) {
	return self->num_procs;
}

static double get_total(ullmf_distribution_t * self) {
	return self->total;
}

#include <stdio.h>
static void redistribute_remainder(ullmf_distribution_t * self) {
//TODO redistribute remainder if excess != 0
	if (!self->excess)
		return;

	int ordered_ratios_index[self->num_procs];
	for (int i = 0; i < self->num_procs; i++) {
		ordered_ratios_index[i] = i;
	}

	// Insertion sort
    for (int i = 1; i < self->num_procs; i++) {
        int j = i;
        int aux;
        while (j > 0 &&
        	   self->ratios[ordered_ratios_index[j]] < self->ratios[ordered_ratios_index[j - 1]]) {
            aux = ordered_ratios_index[j];
            ordered_ratios_index[j] = ordered_ratios_index[j - 1];
            ordered_ratios_index[j - 1] = aux;
            j--;
        }
    }

    int i = 0;
    while (self->excess != 0) {
    	if (self->excess > 0) {
    		self->ratios[ordered_ratios_index[i]] = self->ratios[ordered_ratios_index[i]] - 0.01;
      		self->excess--;
    	} else {
    		self->ratios[ordered_ratios_index[self->num_procs - i - 1]] =
    				self->ratios[ordered_ratios_index[self->num_procs - i - 1]] + 0.01;
    		self->excess++;
    	}
    	i = (i + 1) % self->num_procs;
    }

	self->total = 0.0;
	for (int i = 0; i < self->num_procs; i++) {
		self->total += self->ratios[i];
	}

    double dbg_ratio_sum = 0;
    dbglog_info("  normalized ratios: ");
    for (int i = 0; i < self->num_procs; i++) {
        dbglog_append( DBG_FMT " ", self->ratios[i] );
        dbg_ratio_sum += self->ratios[i];
    }
    dbglog_append("\n");
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

    _self->excess = 100 - floor(_self->total * 100);
    redistribute_remainder(_self);

    // Functions
    _self->set_ratios = &set_ratios;
    _self->get_num_procs = &get_num_procs;
    _self->get_total = &get_total;
    _self->redistribute_remainder = &redistribute_remainder;
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
