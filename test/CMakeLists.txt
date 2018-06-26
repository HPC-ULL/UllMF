#EML variable determines if EML is installed
function(PREPEND var prefix)
  set(listvar "")
  foreach(f ${ARGN})
    list(APPEND listvar "${prefix}/${f}")
  endforeach(f)
  set(${var} "${listvar}" PARENT_SCOPE)
endfunction(PREPEND)

PREPEND(new_sources "../src" "${sources}")

set(TEST_CASES
  test_ullmf_class_utils
  test_ullmf_distribution
  test_ullmf_measurement_device_dummy
  test_ullmf_measurement_device_mpi
  test_ullmf_measurement_device_eml
  test_ullmf_workload
  
)

foreach(test_case ${TEST_CASES})
  message(STATUS "Building test ${test_case}")
  add_executable (${test_case} ${test_case}.c) 
  target_include_directories(${test_case} PRIVATE ../include)
  target_sources(${test_case} PUBLIC ${new_sources})
  target_link_libraries(${test_case} cunit m ullmf)
  add_test(NAME ${test_case} COMMAND ${test_case})
endforeach(test_case)

