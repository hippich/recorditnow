
set(SRCS
    infojob.cpp
    service.cpp
    youtubeservice.cpp
    youtubevideo.cpp
    blipservice.cpp
    blipvideo.cpp)

set(HEADERS service.h
    youtubeservice.h 
    youtubevideo.h
    blipservice.h
    blipvideo.h)


kde4_add_library(kyoublip SHARED ${SRCS})
target_link_libraries(kyoublip ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS})
install(TARGETS kyoublip DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})     
install(FILES ${HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/kyoublip COMPONENT Devel)


