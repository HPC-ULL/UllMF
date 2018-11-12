cmake_minimum_required (VERSION 3.7)
project (ullmf VERSION 0.1)
# gnu lib tree
include(GNUInstallDirs)
include(CTest)

configure_file(config/ullmfConfig.h.in include/ullmfConfig.h @ONLY)
configure_file(config/ullmf.pc.in ullmf.pc @ONLY)

set(CMAKE_MODULE_PATH
    ${CMAKE_MODULE_PATH}
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
)

if(CMAKE_COMPILER_IS_GNUCXX)
    add_definitions(-std=c99)
endif()

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/config")

set(sources
	ullmf_calibration.c
	ullmf_class_utils.c
	ullmf_distribution.c
	ullmf_measurement_device.c
	ullmf_measurement_device_dummy.c
	ullmf_measurement_device_mpi.c
	ullmf_strategy.c
	ullmf_strategy_calibrate.c
	ullmf_strategy_heuristic.c
	ullmf_strategy_heuristic_time.c
	ullmf_workload.c
	ullmf_timer.c
	ullmf_utils.c
	ullmf.c
)

find_package(MPI REQUIRED)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

find_package(EML)
if (EML)
  set(sources ${sources} 
    ullmf_measurement_device_eml.c
    ullmf_strategy_heuristic_energy.c
  )
endif(EML)


FIND_PATH(CUNIT_INCLUDE_DIR NAMES CUnit/CUnit.h)
MARK_AS_ADVANCED(CUNIT_INCLUDE_DIR)

FIND_LIBRARY(CUNIT_LIBRARY NAMES 
    cunit
    libcunit
    cunitlib
)
MARK_AS_ADVANCED(CUNIT_LIBRARY)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CUnit DEFAULT_MSG CUNIT_LIBRARY CUNIT_INCLUDE_DIR)

IF(CUNIT_FOUND)
  SET(CUNIT_LIBRARIES ${CUNIT_LIBRARY})
  SET(CUNIT_INCLUDE_DIRS ${CUNIT_INCLUDE_DIR})
  add_subdirectory(test)
ENDIF(CUNIT_FOUND)

add_subdirectory(src)

