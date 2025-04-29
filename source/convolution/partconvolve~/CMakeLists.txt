include(${CMAKE_CURRENT_SOURCE_DIR}/../../../AH_Externals_Dependencies/max-sdk-base/script/max-pretarget.cmake)


add_msp_external(
    LINK_LIBS
        ibuffer_access
        hisstools
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../AH_Externals_Dependencies/max-sdk-base/script/max-posttarget.cmake)

