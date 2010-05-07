add_subdirectory(recordmydesktop)
add_subdirectory(screenshot)

if(FFMPEG_FOUND)
    add_subdirectory(ffmpegrecorder)
endif()


macro_optional_find_package(AV)
macro_log_feature(AV_FOUND "ffmpeg libav*"
    "Free software/open source LGPL-licensed library of codecs for encoding and decoding video and audio data."
    "http://ffmpeg.org/")

macro_optional_find_package(LZO)
macro_log_feature(LZO_FOUND "lzo"
    "Very fast compression library."
    "http://www.oberhumer.com/opensource/lzo/")

    
if (AV_FOUND AND XFIXES_FOUND AND LZO_FOUND)
    add_subdirectory(kasti)
endif(AV_FOUND AND XFIXES_FOUND AND LZO_FOUND)
