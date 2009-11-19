include_directories(${CMAKE_SOURCE_DIR}/src/plugins/upload
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES})

add_subdirectory(icons)

set(SRCS
    youtubeuploader.cpp)


kde4_add_ui_files(SRCS ui/youtube.ui)
kde4_add_kcfg_files(SRCS recorditnow_youtube.kcfgc)
kde4_add_plugin(recorditnow_youtube ${SRCS})
target_link_libraries(recorditnow_youtube ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} recorditnow_uploader recorditnow_plugin ${KDE4_KIO_LIBS} kyoublip)
install(TARGETS recorditnow_youtube DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES recorditnow_youtube.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES recorditnow_youtube.kcfg  DESTINATION  ${KCFG_INSTALL_DIR})
