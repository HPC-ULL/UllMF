cmake_minimum_required (VERSION 3.9)
project (ullmf VERSION 0.1 DESCRIPTION "Ull Multiobjective Framework")
# gnu lib tree
include(GNUInstallDirs)
include(CTest)

configure_file(config/ullmfConfig.h.in include/ullmfConfig.h @ONLY)
configure_file(config/ullmf.pc.in ullmf.pc @ONLY)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/config")

set(sources
  ullmf_class_utils.c
  ullmf_measurement_device.c
  ullmf_measurement_device_dummy.c
  ullmf_strategy.c
  ullmf_strategy_calibrate.c
)

find_library(EML eml)
if (EML)
  set(sources ${sources} ullmf_measurement_device_eml.c)
endif(EML)
find_package(MPI)

find_package(MPI)
if (MPI_FOUND)
  set(sources ${sources} ullmf_measurement_device_mpi.c)
  include_directories(SYSTEM ${MPI_INCLUDE_PATH})
endif(MPI_FOUND)

add_subdirectory(test)
add_subdirectory(src)

