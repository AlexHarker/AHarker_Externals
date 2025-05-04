include(${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/max-sdk-base/script/max-pretarget.cmake)

add_msp_external(
	PROJECT_SOURCE
		${CMAKE_CURRENT_SOURCE_DIR}/../valconvert/valconvert.cpp
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../dependencies/max-sdk-base/script/max-posttarget.cmake)