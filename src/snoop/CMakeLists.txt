


set(SRCS
    event.cpp
    manager.cpp
    device.cpp)


# snoop
kde4_add_library(recorditnow_snoop SHARED ${SRCS})
target_link_libraries(recorditnow_snoop ${KDE4_KDECORE_LIBS})
install(TARGETS recorditnow_snoop ${INSTALL_TARGETS_DEFAULT_ARGS})
