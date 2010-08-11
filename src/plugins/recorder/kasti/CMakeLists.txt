include_directories(${CMAKE_SOURCE_DIR}/src/)
add_definitions(-D__STDC_CONSTANT_MACROS)

set(SRCS kastirecorder.cpp
    kastiencoder.cpp)

set(CONFIG_SRCS
    kasticonfig.cpp)


set(LIBS ${KDE4_KDECORE_LIBS} 
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS} 
    ${AV_LIBS} 
    ${X11_Xfixes_LIB} 
    ${X11_LIBRARIES} 
    ${LZO_LIBRARIES}
    recorditnow_frame)

# Recorder
kde4_add_kcfg_files(SRCS recorditnow_kasti.kcfgc)
kde4_add_plugin(recorditnow_kasti ${SRCS})
target_link_libraries(recorditnow_kasti recorditnow_recorder recorditnow_plugin ${LIBS})
install(TARGETS recorditnow_kasti DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES recorditnow_kasti.desktop DESTINATION ${SERVICES_INSTALL_DIR})

# Cfg
kde4_add_ui_files(CONFIG_SRCS ui/kasticonfig.ui)
kde4_add_kcfg_files(CONFIG_SRCS recorditnow_kasti.kcfgc)
kde4_add_plugin(recorditnow_kasti_config ${CONFIG_SRCS})
target_link_libraries(recorditnow_kasti_config ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS})               
install(TARGETS recorditnow_kasti_config DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES recorditnow_kasti_config.desktop DESTINATION ${SERVICES_INSTALL_DIR})   
install(FILES recorditnow_kasti.kcfg  DESTINATION  ${KCFG_INSTALL_DIR})

