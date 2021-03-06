find_package(AvCodec)
find_package(AVFormat)
find_package(AVUtil)
find_package(SWScale)

if (AVCODEC_FOUND AND AVFORMAT_FOUND AND AVUTIL_FOUND AND SWSCALE_FOUND)
    set(AV_FOUND TRUE)
    set(AV_LIBS ${AVCODEC_LIBRARIES} ${AVFORMAT_LIBRARIES} ${AVUTIL_LIBRARIES} ${SWSCALE_LIBRARIES})
    set(AV_INCLUDE ${AVCODEC_INCLUDE_DIR} ${AVFORMAT_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} ${SWSCALE_INCLUDE_DIR})
else()
    set(AV_FOUND FALSE)
endif()
    