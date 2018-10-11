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
#include "ullmf_class_utils.h"
#include "ullmf_distribution.h"
#include "ullmf_strategy.h"
#include "ullmf_workload.h"


int init_suite1(void)
{
   return 0;
}


int clean_suite1(void)
{
   return 0;
}

void test_redistribute(void)
{
	int counts[4] = {5000, 5000, 5000, 5000};
	int total = 20000;
	int displs[4] = {0, 5000, 10000, 15000};
	int num_procs = 4;
	int blocksize = 8;
	ullmf_workload_t * workload = _new(Workload, num_procs, counts, displs, blocksize);

	double ratios[4] = {0.10, 0.20, 0.30, 0.40};
	ullmf_distribution_t * new_distribution = _new(Distribution, num_procs, ratios);

	ullmf_calibration_t calib;
	ullmf_strategy_t strategy = {
		._class.size = sizeof(ullmf_strategy_t),
		._class.name = ullmf_strategy_class,
		._class.constructor = 0,
		._class.destructor = 0,
		.redistribute = &ullmf_strategy_redistribute,
		.best_candidate = new_distribution,
	};

	calib.workload = workload;
	calib.strategy = &strategy;

	calib.strategy->redistribute(&calib);

	CU_ASSERT_EQUAL(0.10 * total, calib.workload->counts[0]);
	CU_ASSERT_EQUAL(0.20 * total, calib.workload->counts[1]);
	CU_ASSERT_EQUAL(0.30 * total, calib.workload->counts[2]);
	CU_ASSERT_EQUAL(0.40 * total, calib.workload->counts[3]);

	CU_ASSERT_EQUAL( calib.workload->counts[0] % blocksize, 0);
	CU_ASSERT_EQUAL( calib.workload->counts[1] % blocksize, 0);
	CU_ASSERT_EQUAL( calib.workload->counts[2] % blocksize, 0);
	CU_ASSERT_EQUAL( calib.workload->counts[3] % blocksize, 0);

	total = 11000;
	int counts2[4] = {3000, 3000, 3000, 2000};
	int displs2[4] = {0, 3000, 6000, 9000};
	double proportional_workload2[4] = {0.09, 0.2, 0.30, 0.40};
	calib.workload->set_workload(calib.workload, counts2, displs2);
	new_distribution->set_proportional_workload(new_distribution, proportional_workload2);

	calib.strategy->redistribute(&calib);
	CU_ASSERT_EQUAL(calib.workload->counts[0] % blocksize, 0);
	CU_ASSERT_EQUAL(calib.workload->counts[1] % blocksize, 0);
	CU_ASSERT_EQUAL(calib.workload->counts[2] % blocksize, 0);
	CU_ASSERT_EQUAL(calib.workload->counts[3] % blocksize, 0);

	_delete(calib.workload);
	_delete(new_distribution);
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
   if ((NULL == CU_add_test(pSuite, "test of workload redistribute()", test_redistribute))
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


