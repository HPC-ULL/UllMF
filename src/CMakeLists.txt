add_library(ullmf SHARED ${sources})

if (EML)
  target_link_libraries(ullmf eml confuse) 
endif(EML)

find_package(MPI)
if (MPI_FOUND)
  target_link_libraries(ullmf ${MPI_C_LIBRARIES})
endif(MPI_FOUND)

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
