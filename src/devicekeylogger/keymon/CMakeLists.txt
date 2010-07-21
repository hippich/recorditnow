set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})

if (${KDE_VERSION} VERSION_LESS "4.4.2")
    set(KAUTH_HELPER_BACKEND_NAME "DBUS")
endif(${KDE_VERSION} VERSION_LESS "4.4.2")

# helper
set(HELPER_SRCS
    devicehelper.cpp
    device.cpp)

kde4_add_executable(recorditnow_helper ${HELPER_SRCS})
target_link_libraries(recorditnow_helper ${KDE4_KDECORE_LIBS} recorditnow_keylogger recorditnow_devicekeylogger)
install(TARGETS recorditnow_helper DESTINATION ${LIBEXEC_INSTALL_DIR})

kde4_install_auth_helper_files(recorditnow_helper org.kde.recorditnow.helper root)
kde4_install_auth_actions(org.kde.recorditnow.helper helper.actions)


