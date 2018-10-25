/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 *        File: test_ullmf_strategy_heuristic.c
 *  Created on: Oct 11, 2018
 *      Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#include "CUnit/Basic.h"
#include "ullmf_calibration.h"
#include "ullmf_strategy_heuristic.h"
#include "ullmf_strategy_heuristic_time.h"
#include "ullmf_strategy_heuristic_energy.h"
#include "ullmf_workload.h"


static const float error_tolerance = 1e-9;

int init_suite1(void)
{
   return 0;
}


int clean_suite1(void)
{
   return 0;
}


void test_ullmf_evalue_sum(void)
{
	int num_procs = 2;
	int counts[2] = {200, 200};
	int displ[2] = {0, 200};
	double ratios[2] = {1, 2};
	ullmf_calibration_t calib;
	calib.num_procs = num_procs;
    calib.workload = _new(Workload, num_procs, counts, displ, 1);
    double sum = ullmf_evalue_sum(&calib, calib.workload, ratios);
    CU_ASSERT_DOUBLE_EQUAL(sum, 600.0, error_tolerance);
    _delete(calib.workload);

}


void test_ullmf_evalue_max(void)
{
	int num_procs = 2;
	int counts[2] = {200, 200};
	int displ[2] = {0, 100};
	double ratios[2] = {1, 2};
	ullmf_calibration_t calib;
	calib.num_procs = num_procs;
    calib.workload = _new(Workload, num_procs, counts, displ, 1);
    double max = ullmf_evalue_max(&calib, calib.workload, ratios);
    CU_ASSERT_DOUBLE_EQUAL(max, 400.0, error_tolerance);
    _delete(calib.workload);
}


void test_is_movement_legal(void)
{
	double movement = 0.25;
	double current_ratio = 0.2;
	int direction = 1;
	CU_ASSERT_TRUE(is_movement_legal(current_ratio, movement, direction));
    direction = -1;
    CU_ASSERT_FALSE(is_movement_legal(current_ratio, movement, direction));

	current_ratio = 0.8;
	direction = 1;
	CU_ASSERT_FALSE(is_movement_legal(current_ratio, movement, direction));
	direction = -1;
	CU_ASSERT_TRUE(is_movement_legal(current_ratio, movement, direction));
}


void test_get_resource_ratio(void)
{
	double resource_consumption = 2.00;
	double counts = 10;
	CU_ASSERT_DOUBLE_EQUAL(get_resource_ratio(resource_consumption, counts), 0.2, error_tolerance);
}


void test_move_workload(void)
{
	int num_procs = 3;
	int counts[3] = {300, 300, 400};
	int displ[3] = {0, 300, 600};
	double ratios[3] = {0.3, 0.3, 0.4};
	int blocksize = 1;
	ullmf_calibration_t calib;
	calib.num_procs = num_procs;
    calib.workload = _new(Workload, num_procs, counts, displ, blocksize);
    calib.strategy = ullmf_strategy_heuristic_time;
    ullmf_strategy_heuristic_t *heuristic = (ullmf_strategy_heuristic_t *) calib.strategy;
    heuristic->search_distance = 0.2;

	ullmf_workload_t *moved_workload;
	moved_workload = move_workload(&calib, 0, 1);
	CU_ASSERT_DOUBLE_EQUAL(moved_workload->proportional_workload[0], 0.50, error_tolerance);
	CU_ASSERT_DOUBLE_EQUAL(moved_workload->proportional_workload[1], 0.20, error_tolerance);
	CU_ASSERT_DOUBLE_EQUAL(moved_workload->proportional_workload[2], 0.30, error_tolerance);
	_delete(moved_workload);

	moved_workload = move_workload(&calib, 0, -1);
	CU_ASSERT_DOUBLE_EQUAL(moved_workload->proportional_workload[0], 0.10, error_tolerance);
	CU_ASSERT_DOUBLE_EQUAL(moved_workload->proportional_workload[1], 0.40, error_tolerance);
	CU_ASSERT_DOUBLE_EQUAL(moved_workload->proportional_workload[2], 0.50, error_tolerance);
	_delete(moved_workload);
	_delete(calib.workload);


	int counts2[3] = {300, 100, 600};
	int displ2[3] = {0, 300, 400};
	double ratios2[3] = {0.3, 0.1, 0.6};
    heuristic->search_distance = 0.3;
    calib.workload = _new(Workload, num_procs, counts2, displ2, blocksize);

	moved_workload = move_workload(&calib, 0, 1);
	CU_ASSERT_DOUBLE_EQUAL(moved_workload->proportional_workload[0], 0.60, error_tolerance);
	CU_ASSERT_DOUBLE_EQUAL(moved_workload->proportional_workload[1], 0.025, error_tolerance);
	CU_ASSERT_DOUBLE_EQUAL(moved_workload->proportional_workload[2], 0.375, error_tolerance);
	_delete(moved_workload);

	moved_workload = move_workload(&calib, 0, -1);
	CU_ASSERT_DOUBLE_EQUAL(moved_workload->proportional_workload[0], 0.00, error_tolerance);
	CU_ASSERT_DOUBLE_EQUAL(moved_workload->proportional_workload[1], 0.25, error_tolerance);
	CU_ASSERT_DOUBLE_EQUAL(moved_workload->proportional_workload[2], 0.75, error_tolerance);
	_delete(moved_workload);
	_delete(calib.workload);
}


void test_generate_distributions(void)
{
	int num_procs = 3;
	int counts[3] = {300, 100, 600};
	int displ[3] = {0, 300, 400};
	double ratios[3] = {0.3, 0.1, 0.6};
	int blocksize = 1;
	ullmf_calibration_t calib;
	calib.num_procs = num_procs;
    calib.workload = _new(Workload, num_procs, counts, displ, blocksize);
    calib.strategy = ullmf_strategy_heuristic_time;
    ullmf_strategy_heuristic_t *heuristic = (ullmf_strategy_heuristic_t *) calib.strategy;
    heuristic->search_distance = 0.3;

    ullmf_workload_t ** candidates;
    int num_candidates = generate_distributions(&calib, &candidates);
    // Expected
    //	To 0; 0.30 0.10 0.60  -> 0.600 0.025 0.375
    //	To 1; 0.30 0.10 0.60  -> 0.150 0.400 0.450
    //	To 2; 0.30 0.10 0.60  -> 0.075 0.015 0.910
    //	From 0; 0.30 0.10 0.60  -> 0.000 0.250 0.750
    //	From 1; 0.30 0.10 0.60  -> 0.450 -0.200 0.750 <- Discarded
    //	From 2; 0.30 0.10 0.60  -> 0.450 0.250 0.300
    int expected_candidates = 5;
    double expected[6][3] = {
    		{0.600, 0.025, 0.375},
    		{0.150, 0.400, 0.450},
    		{0.075, 0.015, 0.910},
    		{0.000, 0.250, 0.750},
    		{0.450, 0.250, 0.300},
    };
    for (int i = 0; i < num_candidates; i++) {
    	for(int j = 0; j < num_procs; j++) {
    	    CU_ASSERT_DOUBLE_EQUAL(candidates[i]->proportional_workload[j], expected[i][j], error_tolerance);
    	}
		_delete(candidates[i]);
    }
    CU_ASSERT_EQUAL(num_candidates, expected_candidates);

	_delete(calib.workload);
}

void test_free_distributions(void)
{
	int num_procs = 3;
	int counts[3] = {300, 100, 600};
	int displ[3] = {0, 300, 400};
	double ratios[3] = {0.3, 0.1, 0.6};
	int blocksize = 1;
	ullmf_calibration_t calib;
	calib.num_procs = num_procs;
    calib.workload = _new(Workload, num_procs, counts, displ, blocksize);
    calib.strategy = ullmf_strategy_heuristic_time;
    ullmf_strategy_heuristic_t *heuristic = (ullmf_strategy_heuristic_t *) calib.strategy;
    heuristic->search_distance = 0.3;

    ullmf_workload_t ** candidates;
    int num_candidates = generate_distributions(&calib, &candidates);
    CU_ASSERT_NOT_EQUAL(candidates, 0);
    free_distributions(num_candidates, &candidates);
    CU_ASSERT_EQUAL(candidates, 0);

	_delete(calib.workload);
}

void test_heuristic_search(void) {
	int num_procs = 3;
	int counts[3] = {300, 100, 600};
	int displ[3] = {0, 300, 400};
	double ratios[3] = {0.3, 0.1, 0.6};
	double measurements[3] = {300, 200, 300};
	// 	 0.300 0.100 0.600 = {300, 200, 300} = 800
	//	 0.600 0.025 0.375 = {600, 50, 187.5} = 837.5
	//	 0.150 0.400 0.450 = {150, 800, 225} = 1175
	//	 0.075 0.015 0.910 = {75, 30, 455} = 560 <- Best candidate
	//	 0.000 0.250 0.750 = {0, 500, 375} = 875
	//	 0.450 0.250 0.300 = {450, 500, 150} 1100
	int blocksize = 1;
	double search_distance = 0.3;
	ullmf_calibration_t calib;
	calib.num_procs = num_procs;
    calib.workload = _new(Workload, num_procs, counts, displ, blocksize);
    calib.measurements = measurements;
    calib.strategy = ullmf_strategy_heuristic_energy;
    ullmf_strategy_heuristic_t *heuristic = (ullmf_strategy_heuristic_t *) calib.strategy;
    heuristic->search_distance = search_distance;

    heuristic_search(&calib);

    double expected[3] = {0.075, 0.015, 0.910};
    for (int i = 0; i < num_procs; i++) {
    	CU_ASSERT_DOUBLE_EQUAL(calib.strategy->best_candidate->proportional_workload[i],
    			expected[i], error_tolerance);
    }
    CU_ASSERT_TRUE(heuristic->moved);
    CU_ASSERT_TRUE(heuristic->search_distance < search_distance);

    calib.strategy = ullmf_strategy_heuristic_time;
    heuristic = (ullmf_strategy_heuristic_t *) calib.strategy;

    heuristic->search_distance = search_distance;
    heuristic_search(&calib);
    for (int i = 0; i < num_procs; i++) {
    	CU_ASSERT_DOUBLE_EQUAL(calib.strategy->best_candidate->proportional_workload[i],
    			ratios[i], error_tolerance);
    }
    CU_ASSERT_FALSE(heuristic->moved);
    CU_ASSERT_TRUE(heuristic->search_distance < search_distance);

    _delete(calib.workload);
    _delete(calib.strategy->best_candidate);
    calib.strategy->best_candidate = 0;
}


void test_calibrate(void)
{
	int num_procs = 2;
	int counts[2] = {100, 100};
	int displ[2] = {0, 100};
	double total, min, max;
    double search_distance1 = 0.25;
    double search_distance2 = 0.125;
    double search_threshold = 0.20;
	ullmf_calibration_t calib;
	calib.id = 0;
	calib.num_procs = num_procs;
    calib.workload = _new(Workload, num_procs, counts, displ, 1);
    calib.strategy = ullmf_strategy_heuristic_time;

	double measurements[2] = {50, 100};
    calib.measurements = measurements;

    // Should not calibrate if measuring
    calib.strategy->mdevice->measuring = true;
	enum ullmf_tag tag = calib.strategy->calibrate(&calib);
	CU_ASSERT_EQUAL(tag, ULLMF_TAG_CALIBRATED);

    calib.strategy->mdevice->measuring = false;
    ullmf_strategy_heuristic_t *heuristic = (ullmf_strategy_heuristic_t *) calib.strategy;
    heuristic->search_distance = search_distance1;
    heuristic->search_threshold = search_threshold;

    // Should calibrate if not measuring
	tag = calib.strategy->calibrate(&calib);
	CU_ASSERT_EQUAL(tag, ULLMF_TAG_RECALIBRATING);
    ullmf_distribution_t * distr = calib.strategy->best_candidate;
	CU_ASSERT_EQUAL(distr->get_num_procs(distr), 2);
	CU_ASSERT_DOUBLE_EQUAL(distr->get_total(distr), 1, 0.0001);
	CU_ASSERT_DOUBLE_EQUAL(distr->proportional_workload[0], 0.75, error_tolerance);
	CU_ASSERT_DOUBLE_EQUAL(distr->proportional_workload[1], 0.25, error_tolerance);
    _delete(calib.workload);
    _delete(calib.strategy->best_candidate);
    calib.strategy->best_candidate = 0;

    // Should invert when reached the reset point
    heuristic->search_distance = search_distance2;
    heuristic->search_threshold = search_threshold;
    CU_ASSERT_EQUAL(heuristic->moved, true);
    CU_ASSERT_EQUAL(heuristic->tried_inversion, false);
	tag = calib.strategy->calibrate(&calib);
	CU_ASSERT_EQUAL(tag, ULLMF_TAG_RECALIBRATING);

    // Should do nothing is the reset probability fails
	heuristic->reset_probability = 0.0;
	heuristic->reset_probability_increment = 0.05;
	tag = calib.strategy->calibrate(&calib);
	CU_ASSERT_EQUAL(tag, ULLMF_TAG_CALIBRATED);

    // Should restart is the reset probability is met
    calib.workload = _new(Workload, num_procs, counts, displ, 1);
    calib.measurements = measurements;
	heuristic->reset_probability = 1.0;

	tag = calib.strategy->calibrate(&calib);
	CU_ASSERT_EQUAL(tag, ULLMF_TAG_RECALIBRATING);
	CU_ASSERT_DOUBLE_EQUAL(heuristic->search_distance, heuristic->reset_search_distance / 2, error_tolerance);
	CU_ASSERT_DOUBLE_EQUAL(heuristic->reset_probability, heuristic->initial_reset_probability, error_tolerance);
	distr = calib.strategy->best_candidate;
	CU_ASSERT_EQUAL(distr->get_num_procs(distr), 2);
	CU_ASSERT_DOUBLE_EQUAL(distr->get_total(distr), 1, 0.0001);
	CU_ASSERT_DOUBLE_EQUAL(distr->proportional_workload[0], 0.50 + heuristic->reset_search_distance, error_tolerance);
	CU_ASSERT_DOUBLE_EQUAL(distr->proportional_workload[1], 0.50 - heuristic->reset_search_distance, error_tolerance);
	_delete(calib.workload);
	_delete(calib.strategy->best_candidate);
	calib.strategy->best_candidate = 0;
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
   if (
	   (NULL == CU_add_test(pSuite, "test of ullmf_evalue_sum()", test_ullmf_evalue_sum)) ||
	   (NULL == CU_add_test(pSuite, "test of ullmf_evalue_max()", test_ullmf_evalue_max)) ||
	   (NULL == CU_add_test(pSuite, "test of is_movement_legal()", test_is_movement_legal)) ||
	   (NULL == CU_add_test(pSuite, "test of get_resource_ratio()", test_get_resource_ratio)) ||
	   (NULL == CU_add_test(pSuite, "test of move_workload()", test_move_workload)) ||
	   (NULL == CU_add_test(pSuite, "test of generate_distributions()", test_generate_distributions)) ||
	   (NULL == CU_add_test(pSuite, "test of free_distributions()", test_free_distributions)) ||
	   (NULL == CU_add_test(pSuite, "test of test_heuristic_search()", test_heuristic_search)) ||
	   (NULL == CU_add_test(pSuite, "test of test_calibrate()", test_calibrate))
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


