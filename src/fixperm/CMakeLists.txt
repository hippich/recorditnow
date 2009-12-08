project(recorditnow_fix_permissions)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

set(KDE_MIN_VERSION "4.3.0")

set(SRCS
   main.cpp
   device.cpp)


# app
kde4_add_executable(recorditnow_fix_permissions ${SRCS})
target_link_libraries(recorditnow_fix_permissions ${KDE4_KDEUI_LIBS} )
install(TARGETS recorditnow_fix_permissions ${INSTALL_TARGETS_DEFAULT_ARGS})

