/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf.c
 * Date: 20 jun. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#include <mpi.h>
#include <string.h>
#include <assert.h>
#include <string.h>

#include "debug.h"
#include "ullmf.h"
#include "ullmf_calibration.h"
#include "ullmf_workload.h"
#include "ullmf_class_utils.h"
#include "ullmf_strategy_heuristic.h"

static void calibrate(ullmf_calibration_t * calib) {
    // TODO Optimize
    MPI_Status status;
    if (calib->id == calib->root) {
        const int tag = calib->strategy->calibrate(calib);
        if (tag == ULLMF_TAG_RECALIBRATING) {
        	calib->strategy->redistribute(calib);
        }
        for (int i = 0; i < calib->num_procs; i++)
            if (i != calib->root)
                MPI_Send(NULL, 0, MPI_BYTE, i, tag, calib->comm);
        status.MPI_TAG = tag;
    } else {
        MPI_Recv(NULL, 0, MPI_BYTE, calib->root, MPI_ANY_TAG, calib->comm, &status);
    }

    if (status.MPI_TAG == ULLMF_TAG_RECALIBRATING) {
        MPI_Bcast(calib->workload->counts, calib->num_procs, MPI_INT, calib->root, calib->comm);
        MPI_Bcast(calib->workload->displs, calib->num_procs, MPI_INT, calib->root, calib->comm);
    } else {
        assert(status.MPI_TAG == ULLMF_TAG_CALIBRATED);
    }
}

enum ullmf_error ullmf_mpi_init(ullmf_calibration_t * const calib) {
	enum ullmf_error error;
	if (calib) {
		error = calib->strategy->mdevice->init(calib->strategy->mdevice);
	} else {
		return ULLMF_NOT_INITIALIZED;
	}
    return error;
}

enum ullmf_error ullmf_mpi_shutdown(ullmf_calibration_t * const calib) {
	calib->strategy->mdevice->shutdown(calib->strategy->mdevice);
    return ULLMF_SUCCESS;
}

enum ullmf_error ullmf_mpi_setup(ullmf_calibration_t ** const new_calib,
        const int * const counts,
        const int * const displs,
		const int blocksize,
        ullmf_strategy_t * const strategy,
        const int root,
        const MPI_Comm comm) {

    ullmf_calibration_t* const calib = malloc(((class_t *) Ullmf_calibration)->size);
    calib->_class = *((class_t *) Ullmf_calibration);
    calib->strategy = strategy;
    MPI_Comm_size(comm, &calib->num_procs);
    MPI_Comm_rank(comm, &calib->id);

    calib->workload = _new(Workload, calib->num_procs, counts, displs, blocksize);
    calib->measurements = malloc(calib->num_procs * sizeof(double));
    calib->root = root;
    calib->started = false;
    calib->comm = comm;
#ifndef NDEBUG
    calib->iteration = 0;
#endif
    dbglog_info("ullmf_mpi_setup\n");
    dbglog_info("   Strategy: %p\n", calib->strategy);
    dbglog_info("     Name: %s\n", calib->strategy->_class.name);
    dbglog_info("     Device: %s\n", calib->strategy->mdevice->_class.name);

    *new_calib = calib;
    return ULLMF_SUCCESS;
}

enum ullmf_error ullmf_mpi_free(ullmf_calibration_t * const calib) {
	if (calib->workload != 0)
		_delete(calib->workload);
    // TODO This is real bad, should be moved inside some sort of destructor for the strategies
	if (calib->strategy->best_candidate != 0)
		_delete(calib->strategy->best_candidate);
	if (!strcmp(calib->strategy->_class.name, ullmf_strategy_heuristic_class)) {
	    ullmf_strategy_heuristic_t* heuristic = (ullmf_strategy_heuristic_t*) calib->strategy;
	    if (heuristic->previous_candidate != 0)
	        _delete(heuristic->previous_candidate);
	}
    free(calib->measurements);
    return ULLMF_SUCCESS;
}

enum ullmf_error ullmf_mpi_start(ullmf_calibration_t * const calib) {
	//dbglog_info("[id = %d] ullmf_mpi_start", calib->id);
    if (calib->started) {
    	dbglog_append(" ALREADY STARTED; \n");
        return ULLMF_ALREADY_STARTED;
    }
    //dbglog_append("\n");
    calib->started = true;
    calib->strategy->mdevice->measurement_start(calib->strategy->mdevice);
    return ULLMF_SUCCESS;
}

enum ullmf_error ullmf_mpi_stop(ullmf_calibration_t * const calib, int * counts, int * displs) {
	dbglog_info("[id = %d] ullmf_mpi_stop\n", calib->id);
#ifndef NDEBUG
	if (calib->root) {
	    dbglog_info("     Calib Iteration: %lu\n", calib->iteration++);
        dbglog_info("      Current Counts: ");
        for (int i = 0; i < calib->num_procs; i++) {
            dbglog_info("%d ", calib->workload->counts[i]);
        }
        dbglog_append("\n");
	}
#endif
    if (!calib->started) {
    	dbglog_append("        NOT STARTED \n");
        return ULLMF_NOT_STARTED;
    }

    calib->strategy->mdevice->measurement_stop(calib->strategy->mdevice);
    calib->measurements[calib->id] = calib->strategy->mdevice->measurement;
    calib->workload->set_workload(calib->workload, counts, displs);
    MPI_Gather(&calib->measurements[calib->id], 1, MPI_DOUBLE,
               calib->measurements, 1, MPI_DOUBLE, calib->root, calib->comm);
    calibrate(calib);

    dbglog_info("          New Counts: ");
    for (int i = 0; i < calib->num_procs; i++) {
        dbglog_append("%d ", calib->workload->counts[i]);
    }
    dbglog_append("\n");

    memcpy(counts, calib->workload->counts, sizeof(*counts) * calib->num_procs);
    memcpy(displs, calib->workload->displs, sizeof(*displs) * calib->num_procs);
    calib->started = false;
    return ULLMF_SUCCESS;
}

