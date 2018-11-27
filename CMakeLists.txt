cmake_minimum_required (VERSION 3.7)
project (ullmf VERSION 0.1)
# gnu lib tree
include(GNUInstallDirs)
include(CTest)

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

set(PUBLIC_HEADERS
	${PROJECT_SOURCE_DIR}/include/ullmf.h
	${PROJECT_SOURCE_DIR}/include/ullmf/calibration.h
	${PROJECT_SOURCE_DIR}/include/ullmf/class_utils.h
	${PROJECT_SOURCE_DIR}/include/ullmf/debug.h
	${PROJECT_SOURCE_DIR}/include/ullmf/distribution.h
	${PROJECT_SOURCE_DIR}/include/ullmf/measurement_device_dummy.h
	${PROJECT_SOURCE_DIR}/include/ullmf/measurement_device_eml_edp.h
	${PROJECT_SOURCE_DIR}/include/ullmf/measurement_device_eml.h
	${PROJECT_SOURCE_DIR}/include/ullmf/measurement_device.h
	${PROJECT_SOURCE_DIR}/include/ullmf/measurement_device_mpi.h
	${PROJECT_SOURCE_DIR}/include/ullmf/strategy_calibrate.h
	${PROJECT_SOURCE_DIR}/include/ullmf/strategy.h
	${PROJECT_SOURCE_DIR}/include/ullmf/strategy_heuristic_edp.h
	${PROJECT_SOURCE_DIR}/include/ullmf/strategy_heuristic_energy.h
	${PROJECT_SOURCE_DIR}/include/ullmf/strategy_heuristic.h
	${PROJECT_SOURCE_DIR}/include/ullmf/strategy_heuristic_time.h
	${PROJECT_SOURCE_DIR}/include/ullmf/timer.h
	${PROJECT_SOURCE_DIR}/include/ullmf/utils.h
	${PROJECT_SOURCE_DIR}/include/ullmf/workload.h
)

find_package(MPI REQUIRED)
include_directories(SYSTEM ${MPI_INCLUDE_PATH})

find_package(EML)
if (EML)
  set(sources ${sources} 
    ullmf_measurement_device_eml.c
    ullmf_strategy_heuristic_energy.c
	ullmf_measurement_device_eml_edp.c
	ullmf_strategy_heuristic_edp.c
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

