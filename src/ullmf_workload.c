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
#include <math.h>

#include "ullmf/debug.h"
#include "ullmf/workload.h"
#include "ullmf/class_utils.h"
#include "ullmf/distribution.h"

static void set_workload(ullmf_workload_t * self, const int * const counts, const int * const displs) {
	 size_t memsize = sizeof(self->counts) * self->num_procs;
	 memcpy(self->counts, counts, memsize);
	 memcpy(self->displs, displs, memsize);
	 self->size = self->displs[self->num_procs - 1] + self->counts[self->num_procs - 1];

	 for (int i = 0; i < self->num_procs; i++)
		 self->proportional_workload[i] = self->counts[i] / (double) self->size;
}

static void set_blocksize(ullmf_workload_t * self, const int block_size) {
	self->blocksize = block_size;
}

static ullmf_workload_t * new_from_distribution(ullmf_workload_t* self, ullmf_distribution_t* dist) {
	int * new_counts = malloc(sizeof(self->counts) * self->num_procs);
	int * new_displs = malloc(sizeof(self->displs) * self->num_procs);

	if (self->num_procs > 0) {
		new_counts[0] = dist->proportional_workload[0] * self->size;
		new_displs[0] = 0;
	}

	int block_total_count = self->size / self->blocksize;
	int remaining = block_total_count;

	double best_ratio = dist->proportional_workload[0];
	double worst_ratio = dist->proportional_workload[0];

	int best_processor = 0;
	int worst_processor = 0;
	int assigned_blocks = 0;

	// Transforms ratios into real problem sizes
	for (int i = 0; i < self->num_procs; i++) {
		if (dist->proportional_workload[i] > best_ratio) {
			best_ratio = dist->proportional_workload[i];
			best_processor = i;
		}

		if (dist->proportional_workload[i] < worst_ratio) {
			worst_ratio = dist->proportional_workload[i];
			worst_processor = i;
		}

		assigned_blocks = round(block_total_count * dist->proportional_workload[i]);
		new_counts[i] = assigned_blocks * self->blocksize;
		remaining -= assigned_blocks;
	}

	if (remaining > 0)
		new_counts[best_processor] += remaining * self->blocksize;
	else if (remaining < 0)
		new_counts[worst_processor] += remaining * self->blocksize;

	for (int i = 0; i < self->num_procs - 1; i++)
		new_displs[i + 1] = new_displs[i] + new_counts[i];

	dbglog_info("     new_from_distribution: ");
    for (int i = 0; i < self->num_procs; i++)
        dbglog_append("%d ", new_counts[i]);
    dbglog_append("\n");


	// TODO test new_from_distribution
	ullmf_workload_t * to_return = _new(Workload, self->num_procs, new_counts, new_displs, self->blocksize);
	free(new_counts);
	free(new_displs);
	return to_return;
}


static ullmf_workload_t * copy(ullmf_workload_t* self) {
	return _new(Workload, self->num_procs, self->counts, self->displs, self->blocksize);
}


static void * ullmf_workload_t_constructor(void * self, va_list * args) {
    ullmf_workload_t * _self = self;

    _self->num_procs = va_arg(*args, int);
    int * counts = va_arg(*args, int *);
    int * displs = va_arg(*args, int *);

    size_t memsize = sizeof(_self->counts) * _self->num_procs;
    _self->counts = malloc(memsize);
    memcpy(_self->counts, counts, memsize);

    _self->displs = malloc(memsize);
    memcpy(_self->displs, displs, memsize);

    _self->size = _self->displs[_self->num_procs - 1] + _self->counts[_self->num_procs - 1];
    _self->blocksize = va_arg(*args, int);

    memsize = sizeof(_self->proportional_workload) * _self->num_procs;
    _self->proportional_workload = malloc(memsize);
    for (int i = 0; i < _self->num_procs; i++)
    	_self->proportional_workload[i] = _self->counts[i] / (double) _self->size;

    // Functions
    _self->set_workload = &set_workload;
    _self->set_blocksize = &set_blocksize;
    _self->new_from_distribution = &new_from_distribution;
    _self->copy = &copy;
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
