
set(SRCS
    youtubeservice.cpp)



kde4_add_library(kgdatayoutube SHARED ${SRCS})
target_link_libraries(kgdatayoutube ${KDE4_KDECORE_LIBS} ${KDE4_KIO_LIBS})
install(TARGETS kgdatayoutube DESTINATION ${INSTALL_TARGETS_DEFAULT_ARGS})     


