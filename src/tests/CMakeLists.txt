enable_testing()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR} ${KDE4_INCLUDES})

MACRO(RECORDITNOW_UNIT_TESTS)
    FOREACH(_testname ${ARGN})
        kde4_add_unit_test(${_testname} NOGUI ${_testname}.cpp)
        target_link_libraries(${_testname} ${KDE4_KDECORE_LIBS} ${QT_QTTEST_LIBRARY} recorditnow_plugin)
    ENDFOREACH(_testname)
ENDMACRO(RECORDITNOW_UNIT_TESTS) 


RECORDITNOW_UNIT_TESTS(
    plugintest
    )
