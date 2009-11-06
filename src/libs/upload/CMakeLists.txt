set(SRCS
   abstractuploader.cpp)

kde4_add_library(recorditnow_uploader SHARED ${SRCS})
target_link_libraries(recorditnow_uploader ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} recorditnow_plugin)
install(TARGETS recorditnow_uploader ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES recorditnow_uploader.desktop DESTINATION ${SERVICETYPES_INSTALL_DIR})
