#EML variable determines if EML is installed

set(TEST_CASES
  test_ullmf_measurement_device_dummy
)

foreach(test_case ${TEST_CASES})
  message(STATUS "Building test ${test_case}")
  add_executable (${test_case} ${test_case}.c) 
  target_include_directories(${test_case} PRIVATE ../include)
  target_sources(${test_case} PUBLIC ullmf_measurement_device_dummy.c)
  #target_include_directories(${test_case} PRIVATE ../src)
  target_link_libraries(${test_case} cunit m ullmf)
endforeach(test_case)


