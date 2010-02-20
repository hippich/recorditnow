set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR})


set(SRCS
    event.cpp
    manager.cpp
    deviceinfo.cpp
    device.cpp)

#SNoop
kde4_add_library(recorditnow_snoop SHARED ${SRCS})
target_link_libraries(recorditnow_snoop ${KDE4_KDECORE_LIBS})
install(TARGETS recorditnow_snoop ${INSTALL_TARGETS_DEFAULT_ARGS})

if(${KDE_VERSION} VERSION_GREATER "4.3.80")
    #helper
    set(HELPER_SRCS
        device.cpp
        event.cpp
        manager.cpp
        devicehelper.cpp
        deviceinfo.cpp)

    kde4_add_executable(recorditnow_helper ${HELPER_SRCS})
    target_link_libraries(recorditnow_helper ${KDE4_KDECORE_LIBS})
    install(TARGETS recorditnow_helper DESTINATION ${LIBEXEC_INSTALL_DIR})
    kde4_install_auth_helper_files(recorditnow_helper org.kde.recorditnow.helper root)
    kde4_install_auth_actions(org.kde.recorditnow.helper helper.actions)
endif()
