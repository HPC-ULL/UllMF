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
#include "ullmf_measurement_device_eml.h"
#include "ullmf_measurement_device.h"
#include <unistd.h>
#include <stdbool.h>


int init_suite1(void)
{
	return 0;
}


int clean_suite1(void)
{
   return 0;
}


void test_init(void)
{
    enum ullmf_measurement_error error_code = ullmf_eml_device.parent.init(&ullmf_eml_device);
    CU_ASSERT_NOT_EQUAL(ullmf_eml_device.ndevices, -1);
    CU_ASSERT_EQUAL(error_code, ULLMF_MEASUREMENT_SUCCESS);
}


void test_shutdown(void)
{
    enum ullmf_measurement_error error_code = ullmf_eml_device.parent.shutdown(&ullmf_eml_device);
    CU_ASSERT_EQUAL(ullmf_eml_device.ndevices, -1);
    CU_ASSERT_EQUAL(error_code, ULLMF_MEASUREMENT_SUCCESS);

}


void test_measurement_start(void)
{
    enum ullmf_measurement_error error_code = ullmf_eml_device.parent.measurement_start(&ullmf_eml_device);
    if (ullmf_eml_device.ndevices > 0) {
    	CU_ASSERT_EQUAL(ullmf_eml_device.parent.measuring, true);
    	CU_ASSERT_EQUAL(ullmf_eml_device.interval_calc_started, true);
    	CU_ASSERT_EQUAL(ullmf_eml_device.current_it, 0);
    	CU_ASSERT_EQUAL(error_code, ULLMF_MEASUREMENT_SUCCESS);
    	error_code = ullmf_eml_device.parent.measurement_start(&ullmf_eml_device);
    	CU_ASSERT_EQUAL(error_code, ULLMF_MEASUREMENT_STARTED);
    	CU_ASSERT_EQUAL(ullmf_eml_device.current_it, 0); // Current it managed by stop
    } else {
    	CU_ASSERT_EQUAL(ullmf_eml_device.parent.measuring, false);
    	CU_ASSERT_EQUAL(error_code, ULLMF_MEASUREMENT_INTERNAL_LIBRARY_ERROR);
    }
}


void test_measurement_stop(void)
{
    // Measurement calibration interval
    enum ullmf_measurement_error error_code = ullmf_eml_device.parent.measurement_stop(&ullmf_eml_device);
    if (ullmf_eml_device.ndevices > 0) {
        CU_ASSERT_EQUAL(error_code, ULLMF_MEASUREMENT_RUNNING);
        CU_ASSERT_EQUAL(ullmf_eml_device.parent.measuring, true);

        int interval_end = ullmf_eml_device.internal_calibration_interval;
        for (int i = ullmf_eml_device.current_it;
             !((ullmf_eml_device.current_it % interval_end) == interval_end - 2); i++) {
            error_code = ullmf_eml_device.parent.measurement_start(&ullmf_eml_device);
            CU_ASSERT_EQUAL(error_code,	ULLMF_MEASUREMENT_STARTED);
            CU_ASSERT_EQUAL(ullmf_eml_device.parent.measuring, true);
            CU_ASSERT_EQUAL(ullmf_eml_device.measuring, true);

            error_code = ullmf_eml_device.parent.measurement_stop(&ullmf_eml_device);
            CU_ASSERT_EQUAL(error_code,	ULLMF_MEASUREMENT_RUNNING);
            CU_ASSERT_EQUAL(ullmf_eml_device.parent.measuring, true);
            CU_ASSERT_EQUAL(ullmf_eml_device.measuring, true);
        }
        sleep(1);

        // Real Measurement
        error_code = ullmf_eml_device.parent.measurement_start(&ullmf_eml_device);
        CU_ASSERT_EQUAL(error_code, ULLMF_MEASUREMENT_STARTED);
        CU_ASSERT_EQUAL(ullmf_eml_device.parent.measuring, true);
        CU_ASSERT_EQUAL(ullmf_eml_device.measuring, true);

        error_code = ullmf_eml_device.parent.measurement_stop(&ullmf_eml_device);
        CU_ASSERT_EQUAL(error_code, ULLMF_MEASUREMENT_SUCCESS);
        CU_ASSERT_EQUAL(ullmf_eml_device.measuring, false);
        CU_ASSERT_EQUAL(ullmf_eml_device.parent.measuring, true);
        CU_ASSERT_NOT_EQUAL(ullmf_eml_device.measurement_interval, 0);
        CU_ASSERT_EQUAL(ullmf_eml_device.next_stop,
                ullmf_eml_device.measurement_interval + ullmf_eml_device.internal_calibration_interval - 1);
        CU_ASSERT_EQUAL(ullmf_eml_device.next_start, ullmf_eml_device.internal_calibration_interval - 1);

        error_code = ullmf_eml_device.parent.measurement_start(&ullmf_eml_device);
        CU_ASSERT_EQUAL(error_code, ULLMF_MEASUREMENT_SUCCESS);
        CU_ASSERT_EQUAL(ullmf_eml_device.parent.measuring, true);
        CU_ASSERT_EQUAL(ullmf_eml_device.measuring, true);

        error_code = ullmf_eml_device.parent.measurement_stop(&ullmf_eml_device);
        CU_ASSERT_EQUAL(error_code, ULLMF_MEASUREMENT_RUNNING);
        CU_ASSERT_EQUAL(ullmf_eml_device.parent.measuring, true);
        CU_ASSERT_EQUAL(ullmf_eml_device.measuring, true);


        if (ullmf_eml_device.measurement_interval > 2) {
            interval_end = ullmf_eml_device.next_stop;
            for (int i = ullmf_eml_device.current_it;
                 !((ullmf_eml_device.current_it % interval_end) == interval_end - 2); i++) {
                error_code = ullmf_eml_device.parent.measurement_start(&ullmf_eml_device);
                CU_ASSERT_EQUAL(error_code, ULLMF_MEASUREMENT_STARTED);
                CU_ASSERT_EQUAL(ullmf_eml_device.parent.measuring, true);
                CU_ASSERT_EQUAL(ullmf_eml_device.measuring, true);

                error_code = ullmf_eml_device.parent.measurement_stop(&ullmf_eml_device);
                CU_ASSERT_EQUAL(error_code, ULLMF_MEASUREMENT_RUNNING);
                CU_ASSERT_EQUAL(ullmf_eml_device.parent.measuring, true);
                CU_ASSERT_EQUAL(ullmf_eml_device.measuring, true);

            }
        }
        sleep(1);

        error_code = ullmf_eml_device.parent.measurement_start(&ullmf_eml_device);
        CU_ASSERT_EQUAL(error_code, ULLMF_MEASUREMENT_STARTED);
        CU_ASSERT_EQUAL(ullmf_eml_device.parent.measuring, true);
        CU_ASSERT_EQUAL(ullmf_eml_device.measuring, true);

        error_code = ullmf_eml_device.parent.measurement_stop(&ullmf_eml_device);
        CU_ASSERT_EQUAL(error_code, ULLMF_MEASUREMENT_SUCCESS);
        CU_ASSERT_EQUAL(ullmf_eml_device.parent.measuring, false);
        CU_ASSERT_EQUAL(ullmf_eml_device.measuring, false);

        CU_ASSERT(ullmf_eml_device.parent.measurement > 1e-9);
        // Testing measurement_stop behavior when start is not called
        error_code = ullmf_eml_device.parent.measurement_stop(&ullmf_eml_device);
        CU_ASSERT_EQUAL(error_code, ULLMF_MEASUREMENT_NOT_STARTED);
    } else {
        CU_ASSERT_EQUAL(error_code, ULLMF_MEASUREMENT_INTERNAL_LIBRARY_ERROR);
    }
}


/* The main() function for setting up and running the tests.
 * Returns a CUE_SUCCESS on successful running, another
 * CUnit error code on failure.
 */
int main(int argc, char** argv)
{
   MPI_Init(&argc, &argv);
   CU_pSuite pSuite = NULL;

   /* initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

   /* add a suite to the registry */
   pSuite = CU_add_suite("suite_ullmf_measurement_device_mpi", init_suite1, clean_suite1);
   if (NULL == pSuite) {
      CU_cleanup_registry();
      return CU_get_error();
   }

   /* add the tests to the suite */
   /* NOTE - ORDER IS IMPORTANT */
   if ((NULL == CU_add_test(pSuite, "test of eml init()", test_init)) ||
       (NULL == CU_add_test(pSuite, "test of eml measurement_start()", test_measurement_start)) ||
       (NULL == CU_add_test(pSuite, "test of eml measurement_stop()", test_measurement_stop )) ||
       (NULL == CU_add_test(pSuite, "test of eml shutdown()", test_shutdown))
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
