
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

macro_optional_find_package(LibGData)
macro_log_feature(LIBGDATA_FOUND "libgdata"
    "GLib-based library for accessing online service APIs using the GData protocol"
    "http://code.google.com/p/gdata-python-client/"
    FALSE
    ""
    "")

macro_optional_find_package(LibSoup2)
macro_log_feature(LIBSOUP24_FOUND "libsoup"
    "An HTTP library implementation in C"
    "http://www.gnome.org/"
    FALSE
    ""
    "")

if(LIBGDATA_FOUND)
    if(LIBSOUP24_FOUND)
        add_subdirectory(youtube)
    endif(LIBSOUP24_FOUND)
endif(LIBGDATA_FOUND)
