include(${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/max-sdk-base/script/max-pretarget.cmake)

add_msp_external(
    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../ibufconcatenate~
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/max-sdk-base/script/max-posttarget.cmake)