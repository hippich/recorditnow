add_subdirectory(icons)

include_directories(${LIBGDATA_INCLUDE_DIRS})

set(SRCS
    youtubeuploader.cpp
    uploadthread.cpp
    addaccountdialog.cpp)


kde4_add_ui_files(SRCS ui/youtube.ui ui/youtube_account.ui)
kde4_add_kcfg_files(SRCS recorditnow_youtube.kcfgc)
kde4_add_plugin(recorditnow_youtube ${SRCS})
target_link_libraries(recorditnow_youtube ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} recorditnow_uploader recorditnow_plugin ${LIBGDATA_LIBRARIES} ${KDE4_KIO_LIBS})
install(TARGETS recorditnow_youtube DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES recorditnow_youtube.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES recorditnow_youtube.kcfg  DESTINATION  ${KCFG_INSTALL_DIR})
