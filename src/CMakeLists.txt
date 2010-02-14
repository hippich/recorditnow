include_directories(${CMAKE_SOURCE_DIR}/src/libs/plugin
    ${CMAKE_SOURCE_DIR}/src/libs/recorder
    ${CMAKE_SOURCE_DIR}/src/libs/encoder
    ${CMAKE_SOURCE_DIR}/src/libs/upload
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES})

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

macro_optional_find_package(XFixes)
macro_log_feature(XFIXES_FOUND "XFixes"
    "X.Org Xfixes library"
    "http://xorg.freedesktop.org/")

if (XFIXES_FOUND)
    add_definitions(-DXFIXES_FOUND=true)
endif()


add_subdirectory(libs)
add_subdirectory(plugins)
add_subdirectory(icons)
add_subdirectory(keymon)

macro_display_feature_log()

set(gui_SRCS
   main.cpp
   mainwindow.cpp
   recorditnowpluginmanager.cpp
   configdialog.cpp
   recordermanager.cpp
   encodermanager.cpp
   uploadmanager.cpp
   cursorwidget.cpp
   application.cpp
   mouseconfig.cpp
   zoomview.cpp
   mousebutton.cpp
   keymondialog.cpp
   frame/frame.cpp
   frame/frameinfowidget.cpp
   frame/movewidget.cpp
   timeline/timelinedock.cpp
   timeline/timeline.cpp
   timeline/timelinetopicsdialog.cpp
   timeline/topicwidget.cpp
   timeline/topic.cpp
   timeline/topictreeitem.cpp)


# GUI
kde4_add_ui_files(gui_SRCS 
    ui/toolbarwidget.ui
    ui/settings.ui
    ui/mouse.ui 
    ui/zoom.ui 
    ui/timeline.ui 
    ui/timelinetopics.ui 
    ui/timelineconfig.ui 
    ui/keymondialog.ui)

if (JOSCHYCORE_FOUND)
    add_definitions(-DJOSCHYCORE_FOUND)
    set(gui_SRCS ${gui_SRCS}
        upload/pluginpage.cpp
        upload/uploadwizard.cpp
        upload/videopage.cpp
        upload/accountpage.cpp
        upload/uploadpage.cpp)

    kde4_add_ui_files(gui_SRCS upload/ui/pluginpage.ui
        upload/ui/videopage.ui
        upload/ui/uploadpage.ui
        upload/ui/accountpage.ui)
endif(JOSCHYCORE_FOUND)


kde4_add_kcfg_files(gui_SRCS recorditnow.kcfgc)
kde4_add_executable(recorditnow ${gui_SRCS})
target_link_libraries(recorditnow ${KDE4_KDEUI_LIBS}
                                  ${KDE4_KUTILS_LIBS}
                                  ${KDE4_KIO_LIBS}
                                  recorditnow_recorder
                                  recorditnow_encoder
                                  recorditnow_uploader
                                  recorditnow_snoop
                                  ${X11_Xcursor_LIB})

if (JOSCHYCORE_FOUND)
    target_link_libraries(recorditnow ${JOSCHYCORE_LIBRARIES})
endif(JOSCHYCORE_FOUND)

install(TARGETS recorditnow ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############
install(FILES recorditnow.kcfg  DESTINATION  ${KCFG_INSTALL_DIR})
install(FILES recorditnowui.rc  DESTINATION  ${DATA_INSTALL_DIR}/recorditnow)
