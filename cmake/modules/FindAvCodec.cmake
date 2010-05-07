# - Find AVCodec
# Find the AVCodec library
#
# This module defines
#  AVCODEC_FOUND - whether the JoschyCore library was found
#  AVCODEC_LIBRARIES - the library
#  AVCODEC_INCLUDE_DIR - the include path of the library
#


  
# lib suffix
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    SET(LIB_SUFFIX "")
else(CMAKE_SIZEOF_VOID_P EQUAL 4)
        SET(LIB_SUFFIX 64)
endif()


# libraries
find_library (AVCODEC_LIBRARIES NAMES avcodec PATHS ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})

# include
find_path (AVCODEC_INCLUDE_DIR NAMES libavcodec/avcodec.h PATHS ${INCLUDE_INSTALL_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AVCodec DEFAULT_MSG AVCODEC_LIBRARIES AVCODEC_INCLUDE_DIR)
mark_as_advanced(AVCODEC_INCLUDE_DIR AVCODEC_LIBRARIES)
