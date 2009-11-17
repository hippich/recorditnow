
set(SRCS
    service.cpp
    youtubeservice.cpp
    youtubevideo.cpp)

set(HEADERS service.h
    youtubeservice.h 
    youtubevideo.h)


kde4_add_library(kgdata SHARED ${SRCS})
target_link_libraries(kgdata ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS})
install(TARGETS kgdata DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})     
install(FILES ${HEADERS} DESTINATION ${INCLUDE_INSTALL_DIR}/kgdata COMPONENT Devel)


