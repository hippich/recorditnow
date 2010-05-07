set(SRCS kastirecorder.cpp)
set(LIBS ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${AV_LIBS} ${X11_Xfixes_LIB} ${X11_LIBRARIES})

kde4_add_plugin(recorditnow_kasti ${SRCS})
target_link_libraries(recorditnow_kasti recorditnow_recorder recorditnow_plugin ${LIBS})
install(TARGETS recorditnow_kasti DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES recorditnow_kasti.desktop DESTINATION ${SERVICES_INSTALL_DIR})


