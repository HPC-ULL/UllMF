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

static void set_proportional_workload(ullmf_distribution_t * self,
									  const double * const proportional_workload) {
	size_t memsize = self->num_procs * sizeof(self->proportional_workload);
	memcpy(self->proportional_workload, proportional_workload, memsize);
    self->total = 0;
	for (int i = 0; i < self->num_procs; i++)
		self->total += proportional_workload[i];
	self->redistribute_remainder(self);
}

static int get_num_procs(ullmf_distribution_t * self) {
	return self->num_procs;
}

static double get_total(ullmf_distribution_t * self) {
	return self->total;
}

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
        	   self->proportional_workload[ordered_ratios_index[j]] <
			   self->proportional_workload[ordered_ratios_index[j - 1]]) {
            aux = ordered_ratios_index[j];
            ordered_ratios_index[j] = ordered_ratios_index[j - 1];
            ordered_ratios_index[j - 1] = aux;
            j--;
        }
    }

    int i = 0;
    while (self->excess != 0) {
    	if (self->excess > 0) {
    		self->proportional_workload[ordered_ratios_index[i]] =
    				self->proportional_workload[ordered_ratios_index[i]] - 0.01;
      		self->excess--;
    	} else {
    		self->proportional_workload[ordered_ratios_index[self->num_procs - i - 1]] =
    				self->proportional_workload[ordered_ratios_index[self->num_procs - i - 1]] + 0.01;
    		self->excess++;
    	}
    	i = (i + 1) % self->num_procs;
    }

	self->total = 0.0;
	for (int i = 0; i < self->num_procs; i++) {
		self->total += self->proportional_workload[i];
	}

//    double dbg_ratio_sum = 0;
//    dbglog_info("  normalized ratios: ");
//    for (int i = 0; i < self->num_procs; i++) {
//        dbglog_append( DBG_FMT " ", self->proportional_workload[i] );
//        dbg_ratio_sum += self->proportional_workload[i];
//    }
//    dbglog_append("\n");
}

// constructor(int num_procs, double * ratios)
static void * ullmf_distribution_t_constructor(void * self, va_list * args) {
	ullmf_distribution_t * _self = self;

    _self->num_procs = va_arg(*args, int);
    double * proportional_workload = va_arg(*args, double *);

	size_t memsize = _self->num_procs * sizeof(_self->proportional_workload);
    _self->proportional_workload = malloc(memsize);
	memcpy(_self->proportional_workload, proportional_workload, memsize);

	_self->total = 0;
    for(int i = 0; i < _self->num_procs; i++)
    	_self->total += _self->proportional_workload[i];

    _self->excess = 100 - floor(_self->total * 100);
    redistribute_remainder(_self);

    // Functions
    _self->set_proportional_workload = &set_proportional_workload;
    _self->get_num_procs = &get_num_procs;
    _self->get_total = &get_total;
    _self->redistribute_remainder = &redistribute_remainder;
    return _self;
}

static void * ullmf_distribution_t_destructor(void * self) {
	ullmf_distribution_t* _self = self;

    free(_self->proportional_workload);
    _self->proportional_workload = 0;
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
