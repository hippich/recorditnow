if (BUILD_EXAMPLE)
    add_subdirectory(example)
endif (BUILD_EXAMPLE)

install(FILES recorditnow_script.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
