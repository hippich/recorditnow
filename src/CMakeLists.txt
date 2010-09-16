include_directories(${CMAKE_SOURCE_DIR}/src/libs/plugin
    ${CMAKE_SOURCE_DIR}/src/libs/recorder
    ${CMAKE_SOURCE_DIR}/src/libs/encoder
    ${CMAKE_SOURCE_DIR}/src/libs/keylogger
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES})

if (BUILD_WITH_JOSCHY)
    include_directories(${JOSCHY_SNAPSHOT_INCLUDE_DIR})
endif (BUILD_WITH_JOSCHY)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

# XFixes
macro_optional_find_package(XFixes)
macro_log_feature(XFIXES_FOUND "XFixes"
    "X.Org Xfixes library"
    "http://xorg.freedesktop.org/")

set(HAVE_XFIXES XFIXES_FOUND)
set(HAVE_QTSCRIPT QT_QTSCRIPT_FOUND)
set(BUILD_EXAMPLE true)

configure_file(config-recorditnow.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-recorditnow.h)


add_subdirectory(libs)
add_subdirectory(plugins)
add_subdirectory(icons)
add_subdirectory(keyboard)
add_subdirectory(recordkeylogger)
if (HAVE_QTSCRIPT)
    add_subdirectory(script)
endif (HAVE_QTSCRIPT)
add_subdirectory(tests)

macro_display_feature_log()

set (COMMON_SRCS configitem.cpp)


set (FRAME_SRCS frame/frame.cpp 
    frame/framesize.cpp
    frame/frameinfowidget.cpp 
    frame/movewidget.cpp)


set(gui_SRCS
   main.cpp
   mainwindow.cpp
   pluginmanager.cpp
   recordermanager.cpp
   encodermanager.cpp
   application.cpp
   keymonmanager.cpp
   helper.cpp

   zoom/zoomview.cpp
   zoom/zoomdock.cpp

   timeline/timelinedock.cpp
   timeline/timeline.cpp
   timeline/topicwidget.cpp
   timeline/topicprogressbar.cpp
   timeline/topic.cpp

   upload/pluginpage.cpp
   upload/uploadwizard.cpp
   upload/videopage.cpp
   upload/accountpage.cpp
   upload/termspage.cpp
   upload/uploadpage.cpp

   mouse/mousebutton.cpp
   mouse/cursorwidget.cpp

   keyboard/keyloggerosd.cpp
   keyboard/keyloggerlabel.cpp

   player/playerdock.cpp
   player/videoplayer.cpp
   player/imageplayer.cpp
   player/abstractplayer.cpp
   player/imageframe.cpp
   player/playerdocktitle.cpp

   config/configdialog.cpp
   config/frameconfig.cpp
   config/keyboardconfig.cpp
   config/mouseconfig.cpp
   config/mousebuttonwidget.cpp
   config/configpage.cpp
   config/shortcutsconfig.cpp
   config/zoomconfig.cpp
   config/timelineconfig.cpp
   config/pluginconfig.cpp
   config/notificationconfig.cpp
   config/playerconfig.cpp
   config/soundrow.cpp
   config/colorrow.cpp
   config/topicrow.cpp
   config/framerow.cpp
   config/configosd.cpp

   widgets/osd.cpp
   widgets/checkicon.cpp
   widgets/recordhelpwidget.cpp

   outputwidget.cpp
   timerwidget.cpp
   windowgrabber.cpp
   dockwidget.cpp
   volumeslider.cpp
   listlayout.cpp
   listlayoutrow.cpp

   firstStart/firststartassistant.cpp
   firstStart/welcomepage.cpp
   firstStart/createconfigpage.cpp)

if (HAVE_QTSCRIPT)
    set(gui_SRCS ${gui_SRCS} script/scriptmanager.cpp
                             script/mainwindowscriptadaptor.cpp
                             script/scriptimporter.cpp
                             script/script.cpp
                             script/recorderscriptadaptor.cpp
                             script/encoderscriptadaptor.cpp
                             script/debugscriptadaptor.cpp
                             script/configscriptadaptor.cpp

                             config/scriptdialog.cpp)
endif (HAVE_QTSCRIPT)

# GUI
kde4_add_ui_files(gui_SRCS 
    ui/toolbarwidget.ui
    ui/outputwidget.ui
    ui/timerwidget.ui

    config/ui/settings.ui
    config/ui/mouseconfig.ui 
    config/ui/timelineconfig.ui
    config/ui/frameconfig.ui
    config/ui/keyboardconfig.ui
    config/ui/shortcutsconfig.ui
    config/ui/pluginconfig.ui
    config/ui/playerconfig.ui
    config/ui/notificationconfig.ui
    config/ui/zoom.ui 
    
    widgets/ui/recordhelpwidget.ui

    timeline/ui/timeline.ui 
    timeline/ui/timelinedocktitlewidget.ui
    
    upload/ui/pluginpage.ui
    upload/ui/videopage.ui
    upload/ui/uploadpage.ui
    upload/ui/termspage.ui
    upload/ui/accountpage.ui
        
    zoom/ui/zoomdock.ui
    zoom/ui/zoomdocktitlewidget.ui
    
    firstStart/ui/welcomepage.ui
    firstStart/ui/createconfigpage.ui
    
    player/ui/playerdock.ui
    player/ui/videoplayer.ui
    player/ui/playerdocktitle.ui
    player/ui/volumeslider.ui)

if (HAVE_QTSCRIPT)
kde4_add_ui_files(gui_SRCS
    config/ui/scriptdialog.ui)
endif (HAVE_QTSCRIPT)


# Common
kde4_add_library(recorditnow_common SHARED ${COMMON_SRCS})
target_link_libraries(recorditnow_common ${KDE4_KDEUI_LIBS})
install(TARGETS recorditnow_common ${INSTALL_TARGETS_DEFAULT_ARGS})


# Frame
kde4_add_library(recorditnow_frame SHARED ${FRAME_SRCS})
target_link_libraries(recorditnow_frame ${KDE4_KDEUI_LIBS} recorditnow_common)
install(TARGETS recorditnow_frame ${INSTALL_TARGETS_DEFAULT_ARGS})


set(GUI_SRCS_LINK ${KDE4_KDEUI_LIBS}
                  ${KDE4_KUTILS_LIBS}
                  ${KDE4_KIO_LIBS}
                  ${KDE4_KNOTIFYCONFIG_LIBS}
                  ${KDE4_KDECORE_LIBS}
                  ${KDE4_PHONON_LIBS}
                  ${X11_X11_LIB}
                  ${X11_Xext_LIB}
                  recorditnow_plugin
                  recorditnow_recorder
                  recorditnow_encoder
                  recorditnow_frame
                  recorditnow_common
                  recorditnow_recordkeylogger
                  ${X11_Xcursor_LIB}
                  ${KDE4_PLASMA_LIBS})

if (XFIXES_FOUND)
    set(GUI_SRCS_LINK ${GUI_SRCS_LINK} ${XFIXES_LIBRARIES})
endif(XFIXES_FOUND)

if (HAVE_QTSCRIPT)
    set(GUI_SRCS_LINK ${GUI_SRCS_LINK} ${QT_QTSCRIPT_LIBRARY})
endif (HAVE_QTSCRIPT)

if (BUILD_WITH_JOSCHY)
    set(GUI_SRCS_LINK ${GUI_SRCS_LINK} ${JOSCHY_SNAPSHOT_LIBRARIES})
else (BUILD_WITH_JOSCHY)
    set(GUI_SRCS_LINK ${GUI_SRCS_LINK} ${JOSCHYCORE_LIBRARIES})
endif (BUILD_WITH_JOSCHY)

kde4_add_kcfg_files(gui_SRCS recorditnow.kcfgc)
kde4_add_executable(recorditnow ${gui_SRCS})
target_link_libraries(recorditnow ${GUI_SRCS_LINK})
install(TARGETS recorditnow ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############
install(FILES recorditnow.kcfg  DESTINATION  ${KCFG_INSTALL_DIR})
install(FILES recorditnowui.rc  DESTINATION  ${DATA_INSTALL_DIR}/recorditnow)
