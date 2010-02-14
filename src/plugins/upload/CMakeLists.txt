
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_MODULE_PATH})

if (NOT JOSCHYCORE_FOUND)
    add_subdirectory(lib)
    add_subdirectory(youtube)
    add_subdirectory(blip)
endif (NOT JOSCHYCORE_FOUND)

