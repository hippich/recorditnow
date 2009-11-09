
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

macro_optional_find_package(LibGData)
macro_log_feature(LIBGDATA_FOUND "libgdata"
    "GLib-based library for accessing online service APIs using the GData protocol"
    "http://code.google.com/p/gdata-python-client/"
    FALSE)

if(LIBGDATA_FOUND)
    add_subdirectory(youtube)
endif(LIBGDATA_FOUND)
