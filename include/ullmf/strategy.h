/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: ullmf_strategy.h
 * Date: 20 jun. 2018
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#ifndef ULLMF_STRATEGY_H
#define ULLMF_STRATEGY_H

typedef struct ullmf_strategy ullmf_strategy_t; // Don't move this statement. It avoids a circular dependency

#include "ullmf/class_utils.h"
#include "ullmf/calibration.h"
#include "ullmf/distribution.h"
#include "ullmf/measurement_device.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ullmf_strategy_class "ullmf_strategy"

/**
 * Determines the behavior of the calibrate method. Used in ullmf_mpi_stop()
 */
enum ullmf_tag {
    /// The workload is balanced, it is not necessary to redistribute the counts and displs.
	ULLMF_TAG_CALIBRATED = 0,
	/// The workload has changed, it is necessary to redistribute the work.
	ULLMF_TAG_RECALIBRATING
};

/** Contains state, properties and methods for an strategy */
struct ullmf_strategy {
    /** Object inheritance */
    class_t _class;

    /** Measurement device used by the strategy. It is defined in the concrete strategy. */
    measurement_device_t* mdevice;

    /** Main Calibration Procedure.
     * This method has to be implemented in the concrete strategy.
     * It has to determine how the workload changes based on the metrics gathered in a
     * start stop operation.
     */
    int (*calibrate)(ullmf_calibration_t* calib);

    /** Redistribution procedure.
     * Once the new workload distribution is calculated, the distribution is transformed into a workload,
     * with the corresponding counts and displs.
     */
    void (*redistribute)(ullmf_calibration_t* calib);

    /**
     * Selected workload distribution from the load balancing methods.
     */
    ullmf_distribution_t* best_candidate;
};

/** Generic redistribution.
 *  Transforms the strategy->best_candidate into a valid calib->workload, which calculates the proper counts and displs.
 */
void ullmf_strategy_redistribute(ullmf_calibration_t* calib);

extern const void * Ullmf_strategy;

#ifdef __cplusplus
}
#endif


#endif
