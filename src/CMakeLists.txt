add_subdirectory(libs)
add_subdirectory(plugins)


set(gui_SRCS
   main.cpp
   mainwindow.cpp
   framebox.cpp
   line.cpp)

# GUI
kde4_add_ui_files(gui_SRCS ui/mainwindow.ui ui/toolbarwidget.ui ui/settings.ui)
kde4_add_kcfg_files(gui_SRCS recorditnow.kcfgc)
kde4_add_executable(recorditnow ${gui_SRCS})
target_link_libraries(recorditnow ${KDE4_KDEUI_LIBS} ${KDE4_KUTILS_LIBS} kio recorditnow_recorder)
install(TARGETS recorditnow ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############
install(FILES recorditnow.kcfg  DESTINATION  ${KCFG_INSTALL_DIR})
install(FILES recorditnowui.rc  DESTINATION  ${DATA_INSTALL_DIR}/recorditnow)
