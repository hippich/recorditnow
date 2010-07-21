
# For QtCreator
set(PRIVATE_H keyloggerevent_p.h abstractkeylogger_p.h)

set(lib_SRCS
    ${PRIVATE_H}
    abstractkeylogger.cpp
    keyloggerevent.cpp)

kde4_add_library(recorditnow_keylogger SHARED ${lib_SRCS})

target_link_libraries(recorditnow_keylogger ${KDE4_KDECORE_LIBS}
    ${X11_X11_LIB})


install(TARGETS recorditnow_keylogger ${INSTALL_TARGETS_DEFAULT_ARGS})
