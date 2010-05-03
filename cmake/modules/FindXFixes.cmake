# - Find XFixes
# Find the XFixes library
#
# This module defines
#  XFIXES_FOUND - whether the XFixes library was found
#  XFIXES_LIBRARIES - the library
#  XFIXES_INCLUDE_DIR - the include path of the library
#


find_package(PkgConfig)
if (NOT PKG_CONFIG_FOUND)
    message(STATUS "Could not find pkg-config (pkg name: pkg-config)")
    
    # lib suffix
    if(CMAKE_SIZEOF_VOID_P EQUAL 4)
        SET(LIB_SUFFIX "")
    else(CMAKE_SIZEOF_VOID_P EQUAL 4)
        SET(LIB_SUFFIX 64)
    endif()


    # libraries
    find_library (XFIXES_LIBRARIES NAMES Xfixes PATHS ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})

    # include
    find_path (XFIXES_INCLUDE_DIR NAMES X11/extensions/Xfixes.h PATHS ${INCLUDE_INSTALL_DIR})


    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(XFixes DEFAULT_MSG XFIXES_LIBRARIES XFIXES_INCLUDE_DIR)
    mark_as_advanced(XFIXES_INCLUDE_DIR XFIXES_LIBRARIES)
else()
    pkg_search_module(XFIXES xfixes)
endif (NOT PKG_CONFIG_FOUND)
