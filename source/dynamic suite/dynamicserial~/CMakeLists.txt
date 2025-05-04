include(${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/max-sdk-base/script/max-pretarget.cmake)

add_msp_external(
    INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../Common
    OTHER_SOURCE
    	${CMAKE_CURRENT_SOURCE_DIR}/../Common/patch_slot.cpp
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/max-sdk-base/script/max-posttarget.cmake)