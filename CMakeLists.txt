project(RecordItNow)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

add_definitions( ${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})


########### install files ###############
install(FILES recorditnow.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})


add_subdirectory(src)
