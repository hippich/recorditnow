include_directories(${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_BINARY_DIR}
    ${KDE4_INCLUDES})

# for QtCreator :)
set(SRCS_PRIVATE recordkeylogger_p.h)

set(SRCS ${SRCS_PRIVATE}
    recordkeylogger.cpp)


qt4_automoc(${SRCS})
add_library(recorditnow_recordkeylogger SHARED ${SRCS})
target_link_libraries(recorditnow_recordkeylogger 
        ${KDE4_KDECORE_LIBS}
        ${KDE4_KDEUI_LIBS}
        ${X11_X11_LIB}
        ${X11_Xext_LIB}
        ${X11_XTest_LIB}
	recorditnow_keylogger)

INSTALL(TARGETS recorditnow_recordkeylogger DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})
