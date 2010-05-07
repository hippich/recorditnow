# - Find AVFormat
# Find the AVFormat library
#
# This module defines
#  AVFORMAT_FOUND - whether the JoschyCore library was found
#  AVFORMAT_LIBRARIES - the library
#  AVFORMAT_INCLUDE_DIR - the include path of the library
#


  
# lib suffix
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    SET(LIB_SUFFIX "")
else(CMAKE_SIZEOF_VOID_P EQUAL 4)
        SET(LIB_SUFFIX 64)
endif()


# libraries
find_library (AVFORMAT_LIBRARIES NAMES avformat PATHS ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})

# include
find_path (AVFORMAT_INCLUDE_DIR NAMES libavformat/avformat.h PATHS ${INCLUDE_INSTALL_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(AVFormat DEFAULT_MSG AVFORMAT_LIBRARIES AVFORMAT_INCLUDE_DIR)
mark_as_advanced(AVFORMAT_INCLUDE_DIR AVFORMAT_LIBRARIES)
