# - Find SWScale
# Find the SWScale library
#
# This module defines
#  SWSCALE_FOUND - whether the JoschyCore library was found
#  SWSCALE_LIBRARIES - the library
#  SWSCALE_INCLUDE_DIR - the include path of the library
#


  
# lib suffix
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    SET(LIB_SUFFIX "")
else(CMAKE_SIZEOF_VOID_P EQUAL 4)
        SET(LIB_SUFFIX 64)
endif()


# libraries
find_library (SWSCALE_LIBRARIES NAMES swscale PATHS ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})

# include
find_path (SWSCALE_INCLUDE_DIR NAMES libswscale/swscale.h PATHS ${INCLUDE_INSTALL_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SWScale DEFAULT_MSG SWSCALE_LIBRARIES SWSCALE_INCLUDE_DIR)
mark_as_advanced(SWSCALE_INCLUDE_DIR SWSCALE_LIBRARIES)
