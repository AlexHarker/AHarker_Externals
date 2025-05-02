include(${CMAKE_CURRENT_SOURCE_DIR}/../../../AH_Externals_Dependencies/max-sdk-base/script/max-pretarget.cmake)

add_msp_external(
    INCLUDE_DIRS
        "${CMAKE_SOURCE_DIR}/source/vMSP/Base"
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../AH_Externals_Dependencies/max-sdk-base/script/max-posttarget.cmake)
