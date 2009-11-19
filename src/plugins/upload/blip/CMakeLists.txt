include_directories(${CMAKE_SOURCE_DIR}/src/plugins/upload
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES})

#add_subdirectory(lib)
add_subdirectory(icons)

set(SRCS
    blipuploader.cpp)


kde4_add_ui_files(SRCS ui/blip.ui)
kde4_add_kcfg_files(SRCS recorditnow_blip.kcfgc)
kde4_add_plugin(recorditnow_blip ${SRCS})
target_link_libraries(recorditnow_blip ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} recorditnow_uploader recorditnow_plugin ${KDE4_KIO_LIBS} kyoublip)
install(TARGETS recorditnow_blip DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES recorditnow_blip.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES recorditnow_blip.kcfg DESTINATION  ${KCFG_INSTALL_DIR})
