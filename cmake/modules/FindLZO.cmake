# - Find LZO
# Find the LZO library
#
# This module defines
#  LZO_FOUND - whether the JoschyCore library was found
#  LZO_LIBRARIES - the library
#  LZO_INCLUDE_DIR - the include path of the library
#


  
# lib suffix
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    SET(LIB_SUFFIX "")
else(CMAKE_SIZEOF_VOID_P EQUAL 4)
        SET(LIB_SUFFIX 64)
endif()


# libraries
find_library (LZO_LIBRARIES NAMES lzo2 PATHS ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})

# include
find_path (LZO_INCLUDE_DIR NAMES lzo/lzo1x.h PATHS ${INCLUDE_INSTALL_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LZO DEFAULT_MSG LZO_LIBRARIES LZO_INCLUDE_DIR)
mark_as_advanced(LZO_INCLUDE_DIR LZO_LIBRARIES)
