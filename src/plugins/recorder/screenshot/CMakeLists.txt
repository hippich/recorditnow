set(SRCS
  screenshotrecorder.cpp)

set(confg_SRCS
  screenshotrecorderconfig.cpp)


kde4_add_kcfg_files(SRCS recorditnow_screenshot.kcfgc)
kde4_add_plugin(recorditnow_screenshot ${SRCS})
target_link_libraries(recorditnow_screenshot ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} recorditnow_recorder recorditnow_plugin)
install(TARGETS recorditnow_screenshot DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES recorditnow_screenshot.desktop DESTINATION ${SERVICES_INSTALL_DIR})


kde4_add_ui_files(confg_SRCS ui/screenshot.ui)
kde4_add_kcfg_files(confg_SRCS recorditnow_screenshot.kcfgc)
kde4_add_plugin(recorditnow_screenshot_config ${confg_SRCS})
target_link_libraries(recorditnow_screenshot_config ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})               
install(TARGETS recorditnow_screenshot_config DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES recorditnow_screenshot_config.desktop DESTINATION ${SERVICES_INSTALL_DIR})   
install(FILES recorditnow_screenshot.kcfg  DESTINATION  ${KCFG_INSTALL_DIR})

