include_directories(${CMAKE_SOURCE_DIR}/src/libs/encoder
 ${CMAKE_CURRENT_BINARY_DIR}
 ${CMAKE_BINARY_DIR}
 ${KDE4_INCLUDES})

set(SRCS
  ffmpegencoder.cpp)

set(confg_SRCS
  ffmpegencoderconfig.cpp)


kde4_add_kcfg_files(SRCS ffmpeg.kcfgc)
kde4_add_plugin(recorditnow_ffmpeg ${SRCS})
target_link_libraries(recorditnow_ffmpeg ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} recorditnow_encoder)
install(TARGETS recorditnow_ffmpeg DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES recorditnow_ffmpeg.desktop DESTINATION ${SERVICES_INSTALL_DIR})


kde4_add_ui_files(confg_SRCS ui/ffmpeg.ui)
kde4_add_kcfg_files(confg_SRCS ffmpeg.kcfgc)
kde4_add_plugin(recorditnow_ffmpeg_config ${confg_SRCS})
target_link_libraries(recorditnow_ffmpeg_config ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})               
install(TARGETS recorditnow_ffmpeg_config DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES recorditnow_ffmpeg_config.desktop DESTINATION ${SERVICES_INSTALL_DIR})   
install(FILES ffmpeg.kcfg DESTINATION ${KCFG_INSTALL_DIR})

