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

add_library(ullmf SHARED
  src/calibrate_mpi.c  
)

set_target_properties(ullmf PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "api/ullmf.h")

target_include_directories(ullmf PRIVATE include)
target_include_directories(ullmf PRIVATE src)
install(TARGETS ullmf 
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/ullmf.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
