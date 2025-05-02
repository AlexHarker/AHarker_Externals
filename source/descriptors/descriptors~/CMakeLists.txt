include(${CMAKE_CURRENT_SOURCE_DIR}/../../../AH_Externals_Dependencies/max-sdk-base/script/max-pretarget.cmake)

set(MODULES ${CMAKE_CURRENT_SOURCE_DIR}/modules)

add_msp_external(
	OTHER_SOURCE
		${MODULES}/modules_core.cpp
		${MODULES}/modules_change.cpp
		${MODULES}/modules_content.cpp
		${MODULES}/modules_level.cpp
		${MODULES}/modules_pitch.cpp
		${MODULES}/modules_spectral.cpp
		${MODULES}/modules_summary.cpp
	INCLUDE_DIRS
		${CMAKE_CURRENT_SOURCE_DIR}/library
		${CMAKE_CURRENT_SOURCE_DIR}/modules
    LINK_LIBS
    	ibuffer_access
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../AH_Externals_Dependencies/max-sdk-base/script/max-posttarget.cmake)




