add_library(ullmf SHARED ${sources})

if (EML)
  target_link_libraries(ullmf eml confuse) 
endif(EML)

target_link_libraries(ullmf ${MPI_LIBRARIES})



set_target_properties(ullmf PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION 1)

install(FILES ${PUBLIC_HEADERS} 
        DESTINATION include/ullmf)

target_include_directories(ullmf PRIVATE ../include)
install(TARGETS ullmf 
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(FILES ${CMAKE_BINARY_DIR}/ullmf.pc
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig)
