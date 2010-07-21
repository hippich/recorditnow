add_subdirectory(keymon)

include_directories(${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES})

# for QtCreator :)
set(SRCS_PRIVATE)

set(SRCS ${SRCS_PRIVATE}
    devicekeylogger.cpp
    devicesearchdialog.cpp
    devicesearchwidget.cpp
    deviceinfo.cpp
    manager.cpp)


kde4_add_ui_files(SRCS
        ui/devicewidget.ui
        ui/devicesearchwidget.ui)

qt4_automoc(${SRCS})
add_library(recorditnow_devicekeylogger SHARED ${SRCS})
target_link_libraries(recorditnow_devicekeylogger 
        ${KDE4_KDECORE_LIBS}
        ${KDE4_KDEUI_LIBS}
        ${X11_X11_LIB}
        ${X11_Xext_LIB}
        ${X11_XTest_LIB}
        ${KDE4_SOLID_LIBS}
	recorditnow_keylogger)

INSTALL(TARGETS recorditnow_devicekeylogger DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
