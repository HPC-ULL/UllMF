/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 *        File: test_ullmf_strategy.c
 *  Created on: Jun 28, 2018
 *      Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#include "CUnit/Basic.h"
#include "ullmf/class_utils.h"
#include "ullmf/distribution.h"
#include "ullmf/strategy_calibrate.h"
#include "ullmf/workload.h"


int init_suite1(void)
{
   return 0;
}


int clean_suite1(void)
{
   return 0;
}

void test_calibrate(void)
{
	int num_procs = 2;
	int counts[2] = {200, 200};
	int displ[2] = {0, 100};
	double total, min, max;
	ullmf_calibration_t calib;
	calib.num_procs = num_procs;

    calib.workload = _new(Workload, num_procs, counts, displ, 1);
    calib.strategy = ullmf_strategy_calibrate;

	double measurements[2] = {98, 102};
    calib.measurements = measurements;

	enum ullmf_tag tag = calib.strategy->calibrate(&calib);
	CU_ASSERT_EQUAL(tag, ULLMF_TAG_CALIBRATED);

	double measurements2[2] = {3, 1};
	calib.measurements = measurements2;
	tag = calib.strategy->calibrate(&calib);
	CU_ASSERT_EQUAL(tag, ULLMF_TAG_RECALIBRATING);

    ullmf_distribution_t * distr = calib.strategy->best_candidate;

	CU_ASSERT_EQUAL(distr->get_num_procs(distr), 2);
	CU_ASSERT_DOUBLE_EQUAL(distr->get_total(distr), 1, 0.0001);
	CU_ASSERT_DOUBLE_EQUAL(distr->proportional_workload[0], 0.25, 0.0001);
	CU_ASSERT_DOUBLE_EQUAL(distr->proportional_workload[1], 0.75, 0.0001);

	_delete(distr);
}


/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main()
{
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("suite_ullmf_strategy", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "test of workload calibrate()", test_calibrate))
      )
   {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* Run all tests using the CUnit Basic interface */
   CU_basic_set_mode(CU_BRM_VERBOSE);
   CU_basic_run_tests();
   return CU_get_number_of_failures();
}


