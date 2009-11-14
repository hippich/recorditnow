
set(SRCS
    kblipaccount.cpp
    kblipvideo.cpp)



kde4_add_library(kblip SHARED ${SRCS})
target_link_libraries(kblip ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS})
install(TARGETS kblip DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})     


