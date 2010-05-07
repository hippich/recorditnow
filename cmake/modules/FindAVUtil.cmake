# - Find AVUtil
# Find the AVUtil library
#
# This module defines
#  AVUTIL_FOUND - whether the JoschyCore library was found
#  AVUTIL_LIBRARIES - the library
#  AVUTIL_INCLUDE_DIR - the include path of the library
#


  
# lib suffix
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    SET(LIB_SUFFIX "")
else(CMAKE_SIZEOF_VOID_P EQUAL 4)
        SET(LIB_SUFFIX 64)
endif()


# libraries
find_library (AVUTIL_LIBRARIES NAMES avutil PATHS ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})

# include
find_path (AVUTIL_INCLUDE_DIR NAMES libavutil/avutil.h PATHS ${INCLUDE_INSTALL_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AVUtil DEFAULT_MSG AVUTIL_LIBRARIES AVUTIL_INCLUDE_DIR)
mark_as_advanced(AVUTIL_INCLUDE_DIR AVUTIL_LIBRARIES)
