#add_library(ullmf SHARED calibrate_mpi.c)
file(GLOB sources timer.c ullmf_measurement_device_dummy.c ullmf_class_utils.c)
add_library(ullmf SHARED ${sources})
#target_sources(ullmf PRIVATE ullmf_measurement_device_dummy.c)
#target_sources(ullmf PRIVATE ullmf_class_utils.c)


#if (EML)
#  add_definitions(-DHAVE_EML)
#  target_link_libraries(ullmf eml confuse) 
#endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-DDEBUG)
endif()

set_target_properties(ullmf PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1
    PUBLIC_HEADER "api/ullmf.h")

target_include_directories(ullmf PRIVATE ../include)
#target_include_directories(ullmf PRIVATE .)
install(TARGETS ullmf 
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/ullmf.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
