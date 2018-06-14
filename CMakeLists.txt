cmake_minimum_required (VERSION 3.9)
project (ullmf VERSION 0.1 DESCRIPTION "Ull Multiobjective Framework")
# gnu lib tree
include(GNUInstallDirs)

configure_file(config/ullmfConfig.h.in include/ullmfConfig.h @ONLY)
configure_file(config/ullmf.pc.in ullmf.pc @ONLY)

include_directories (
  "${CMAKE_CURRENT_SOURCE_DIR}"/include
  "${PROJECT_SOURCE_DIR}"/config
)

find_library(EML eml)

add_subdirectory(test)
add_subdirectory(src)

