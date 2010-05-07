add_subdirectory(recordmydesktop)
add_subdirectory(screenshot)

if(FFMPEG_FOUND)
    add_subdirectory(ffmpegrecorder)
endif()


macro_optional_find_package(AV)
macro_log_feature(AV_FOUND "ffmpeg libav*"
    "Free software/open source LGPL-licensed library of codecs for encoding and decoding video and audio data."
    "http://ffmpeg.org/")

if (AV_FOUND)
    add_subdirectory(kasti)
endif(AV_FOUND)
