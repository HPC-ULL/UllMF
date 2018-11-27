/*
 * Copyright (c) 2018 Universidad de La Laguna <cap@pcg.ull.es>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * Author: Alberto Cabrera <Alberto.Cabrera@ull.edu.es>
 */

#include "CUnit/Basic.h"
#include "ullmf/class_utils.h"
#include "ullmf/workload.h"

static ullmf_workload_t* workload1;
static int counts[2] = {2, 2};
static int displs[2] = {0, 2};
static int num_procs = 2;
static int blocksize = 1;

int init_suite1(void)
{
   return 0;
}


int clean_suite1(void)
{
   return 0;
}

void test_new(void)
{
	workload1 = _new(Workload, num_procs, counts, displs, blocksize);
	CU_ASSERT_NOT_EQUAL(workload1, 0);
	CU_ASSERT_EQUAL(workload1->_class, Workload);

	CU_ASSERT_STRING_EQUAL(((class_t*) workload1->_class)->name, ((class_t*) Workload)->name);
	CU_ASSERT_NOT_EQUAL(workload1->counts, 0);
	CU_ASSERT_NOT_EQUAL(workload1->displs, 0);
	CU_ASSERT_NOT_EQUAL(workload1->counts, counts);
	CU_ASSERT_NOT_EQUAL(workload1->displs, displs);
    CU_ASSERT_EQUAL(workload1->num_procs, num_procs);
    int workload_size = 0;
	for (int i = 0; i < workload1->num_procs; i++) {
		CU_ASSERT_EQUAL(counts[i], workload1->counts[i]);
		CU_ASSERT_EQUAL(displs[i], workload1->displs[i]);
		workload_size += counts[i];
	}
    CU_ASSERT_EQUAL(workload1->size, workload_size);
}

void test_delete(void)
{
    _delete(workload1);
	CU_ASSERT_EQUAL(workload1->counts, 0);
	CU_ASSERT_EQUAL(workload1->displs, 0);
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
   pSuite = CU_add_suite("suite_ullmf_workload", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   if ((NULL == CU_add_test(pSuite, "test of workload new()", test_new)) ||
       (NULL == CU_add_test(pSuite, "test of workload delete()", test_delete))
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
