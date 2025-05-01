include(${CMAKE_CURRENT_SOURCE_DIR}/../../../AH_Externals_Dependencies/max-sdk-base/script/max-pretarget.cmake)

add_msp_external(
	PROJECT_SOURCE
		${CMAKE_CURRENT_SOURCE_DIR}/../randomvals/randomvals.cpp
	COMPILE_DEFINITIONS
		"MSP_VERSION"
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../AH_Externals_Dependencies/max-sdk-base/script/max-posttarget.cmake)