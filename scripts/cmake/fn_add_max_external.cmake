unset(CMAKE_OSX_DEPLOYMENT_TARGET)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "requires >= 10.15" FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS True)

function(add_max_external)
    set(options
        DEBUG
        INCLUDE_COMMONSYMS
    )
    set(oneValueArgs 
        PROJECT_NAME
    )
    set(multiValueArgs
        PROJECT_SOURCE
        OTHER_SOURCE
        INCLUDE_DIRS
        LINK_LIBS
        LINK_DIRS
        COMPILE_DEFINITIONS
        COMPILE_OPTIONS
        LINK_OPTIONS
    )

    cmake_parse_arguments(
        AHEXT
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if(NOT DEFINED AHEXT_PROJECT_NAME)
        set(AHEXT_PROJECT_NAME ${PROJECT_NAME})
    endif()

    message(STATUS "PROJECT_NAME: ${AHEXT_PROJECT_NAME}")
    if(DEBUG)
        message(STATUS "PROJECT_SOURCE: ${AHEXT_PROJECT_SOURCE}")
        message(STATUS "OTHER_SOURCE: ${AHEXT_OTHER_SOURCE}")
        message(STATUS "AHEXT_INCLUDE_DIRS: ${AHEXT_INCLUDE_DIRS}")
        message(STATUS "AHEXT_COMPILE_DEFINITIONS: ${AHEXT_COMPILE_DEFINITIONS}")
        message(STATUS "AHEXT_COMPILE_OPTIONS: ${AHEXT_COMPILE_OPTIONS}")
        message(STATUS "AHEXT_LINK_LIBS: ${AHEXT_LINK_LIBS}")
        message(STATUS "AHEXT_LINK_DIRS: ${AHEXT_LINK_DIRS}")
        message(STATUS "AHEXT_LINK_OPTIONS: ${AHEXT_LINK_OPTIONS}")
    endif()

    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum OS X deployment version" FORCE)

    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../../AH_Externals_Package/AHarker_Externals/externals")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../../../AH_Externals_Package/AHarker_Externals/externals" PARENT_SCOPE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" PARENT_SCOPE)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}" PARENT_SCOPE)
    
    set(DEPS_DIR "${CMAKE_SOURCE_DIR}/build/install")
    set(EXTERNAL_DIR "${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/${AHEXT_PROJECT_NAME}.mxo")
    set(SUPPORT_DIR "${CMAKE_SOURCE_DIR}/support")

    if(NOT DEFINED AHEXT_PROJECT_SOURCE)
        file(GLOB AHEXT_PROJECT_SOURCE
            "*.h"
            "*.c"
            "*.cpp"
        )
    endif()

    include_directories( 
        "${MAX_SDK_INCLUDES}"
        "${MAX_SDK_MSP_INCLUDES}"
        "${MAX_SDK_JIT_INCLUDES}"
        "${COMMON_INCLUDES}"
        "${IBUFFER_ACCESS_INCLUDES}"
        "${HISSTOOLS_LIB_INCLUDES}"
    )

    add_library( 
        ${AHEXT_PROJECT_NAME} 
        MODULE
        ${AHEXT_PROJECT_SOURCE}
        ${AHEXT_OTHER_SOURCE}
        $<$<BOOL:${AHEXT_INCLUDE_COMMONSYMS}>:${MAX_SDK_INCLUDES}/common/commonsyms.c>
    )

    target_include_directories(
        ${AHEXT_PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${AHEXT_INCLUDE_DIRS}
        "$<$<PLATFORM_ID:Windows>:${CMAKE_BINARY_DIR}/venv/Library/include>"
    )

    target_compile_definitions(
        ${AHEXT_PROJECT_NAME}
        PRIVATE
        ${AHEXT_COMPILE_DEFINITIONS}
        $<$<BOOL:${AHEXT_INCLUDE_COMMONSYMS}>:INCLUDE_COMMONSYMS>
        $<$<PLATFORM_ID:Darwin>:NDEBUG>
        $<$<PLATFORM_ID:Windows>:NOMINMAX>
        $<$<PLATFORM_ID:Windows>:_USE_MATH_DEFINES>
        $<$<PLATFORM_ID:Windows>:WIN_VERSION>
        $<$<PLATFORM_ID:Windows>:WIN64>
        $<$<PLATFORM_ID:Windows>:_WINDOWS>
        $<$<PLATFORM_ID:Windows>:_USERDLL>
        $<$<PLATFORM_ID:Windows>:WIN_EXT_VERSION>
        $<$<PLATFORM_ID:Windows>:_CRT_SECURE_NO_WARNINGS>
    )

    target_compile_options(
        ${AHEXT_PROJECT_NAME}
        PRIVATE
        ${AHEXT_COMPILE_OPTIONS}
        $<$<PLATFORM_ID:Windows>:/O2>
        $<$<PLATFORM_ID:Windows>:/MT>
        $<$<PLATFORM_ID:Darwin>:-Wmost>
        $<$<PLATFORM_ID:Darwin>:-Wno-four-char-constants>
        $<$<PLATFORM_ID:Darwin>:-Wno-unknown-pragmas>
        $<$<PLATFORM_ID:Darwin>:-Wdeclaration-after-statement>
        $<$<PLATFORM_ID:Darwin>:-fvisibility=hidden>
    )

    target_link_directories(
        ${AHEXT_PROJECT_NAME} 
        PRIVATE
        ${AHEXT_LINK_DIRS}
        $<$<PLATFORM_ID:Windows>:${CMAKE_BINARY_DIR}/venv/Lib>
        $<$<PLATFORM_ID:Windows>:${CMAKE_BINARY_DIR}/venv/Library/lib>
    )

    target_link_options(
        ${AHEXT_PROJECT_NAME}
        PRIVATE
        ${AHEXT_LINK_OPTIONS}
    )

    target_link_libraries(
        ${AHEXT_PROJECT_NAME} 
        PRIVATE
        "${AHEXT_LINK_LIBS}"
        "$<$<PLATFORM_ID:Darwin>:-framework Accelerate>"
        $<$<PLATFORM_ID:Windows>:mkl_core>
        $<$<PLATFORM_ID:Windows>:mkl_intel_lp64>
        $<$<PLATFORM_ID:Windows>:mkl_sequential>
    )

endfunction()
