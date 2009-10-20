include_directories(${CMAKE_SOURCE_DIR}/src/libs/recorder
 ${CMAKE_CURRENT_BINARY_DIR}
 ${CMAKE_BINARY_DIR}
 ${KDE4_INCLUDES})

set(recordmydesktop_SRCS
  recordmydesktoprecorder.cpp)

kde4_add_ui_files(recordmydesktop_SRCS ui/recordmydesktop.ui)
kde4_add_plugin(recorditnow_recordmydesktop ${recordmydesktop_SRCS})
target_link_libraries(recorditnow_recordmydesktop ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} kio recorditnow_recorder)
install(TARGETS recorditnow_recordmydesktop DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES recorditnow_recordmydesktop.desktop DESTINATION ${SERVICES_INSTALL_DIR})

