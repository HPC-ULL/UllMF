/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * File: test_ullmf_distribution.c
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 * Date: 26 jun. 2018
 */

#include "CUnit/Basic.h"
#include "ullmf_class_utils.h"
#include "ullmf_distribution.h"

static ullmf_distribution_t* distribution1 = 0;
static int num_procs = 4;
static double ratios[4] = {0.25, 0.2, 0.25, 0.3};
static double ratios2[4] = {0.2, 0.15, 0.35, 0.35};


int init_suite1(void)
{
   return 0;
}


int clean_suite1(void)
{
   return 0;
}

void test_constructor(void)
{
	distribution1 = _new(Distribution, num_procs, ratios);
	CU_ASSERT_NOT_EQUAL(distribution1, 0);
	CU_ASSERT_EQUAL(distribution1->_class, Distribution);
	CU_ASSERT_STRING_EQUAL(((class_t*) distribution1->_class)->name, ((class_t*) Distribution)->name);
	CU_ASSERT_NOT_EQUAL(distribution1->ratios, 0);
	CU_ASSERT_NOT_EQUAL(distribution1->ratios, ratios);
    CU_ASSERT_EQUAL(distribution1->num_procs, num_procs);
    double total = 0;
	for (int i = 0; i < distribution1->num_procs; i++) {
		CU_ASSERT_DOUBLE_EQUAL(ratios[i], distribution1->ratios[i], 0.005);
		total += ratios[i];
	}
	CU_ASSERT_DOUBLE_EQUAL(distribution1->total, total, 0.005);
}

void test_set_ratios(void)
{
	distribution1->set_ratios(distribution1, num_procs, ratios2);
    double total = 0;
	for (int i = 0; i < distribution1->num_procs; i++) {
		CU_ASSERT_DOUBLE_EQUAL(ratios2[i], distribution1->ratios[i], 0.005);
		total += ratios2[i];
	}
	CU_ASSERT_DOUBLE_EQUAL(distribution1->total, total, 0.005);
}

void test_get_num_procs(void)
{
	CU_ASSERT_EQUAL(distribution1->get_num_procs(distribution1), distribution1->num_procs);
}

void test_get_total(void)
{
	CU_ASSERT_DOUBLE_EQUAL(distribution1->get_total(distribution1), distribution1->total, 0.005);
}

void test_destructor(void)
{
	_delete(distribution1);
	CU_ASSERT_EQUAL(distribution1->ratios, 0);
	CU_ASSERT_EQUAL(distribution1->total, 0);
	CU_ASSERT_EQUAL(distribution1->num_procs, 0);
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
   pSuite = CU_add_suite("suite_ullmf_distribution", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "test of distribution constructor()", test_constructor)) ||
	   (NULL == CU_add_test(pSuite, "test of distribution set_ratios()", test_set_ratios)) ||
	   (NULL == CU_add_test(pSuite, "test of distribution get_num_procs()", test_get_num_procs)) ||
	   (NULL == CU_add_test(pSuite, "test of distribution get_total()", test_get_total)) ||
	   (NULL == CU_add_test(pSuite, "test of distribution destructor()", test_destructor))
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
