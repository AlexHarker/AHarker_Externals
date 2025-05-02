include(${CMAKE_CURRENT_SOURCE_DIR}/../../../AH_Externals_Dependencies/max-sdk-base/script/max-pretarget.cmake)

set(EM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../entrymatcher)

add_msp_external(
	INCLUDE_COMMONSYMS
	OTHER_SOURCE
		${EM_DIR}/database_view.cpp
		${EM_DIR}/entries.cpp
		${EM_DIR}/entry_database.cpp
		${EM_DIR}/matchers.cpp
	INCLUDE_DIRS
		${EM_DIR}
)

include(${CMAKE_CURRENT_SOURCE_DIR}/../../../AH_Externals_Dependencies/max-sdk-base/script/max-posttarget.cmake)
