
cmake_minimum_required(VERSION 3.16)
project(libex C)

set(CMAKE_CONFIGURATION_TYPES Debug Release RelWithDebInfo MinSizeRel)
if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE $ENV{CMAKE_BUILD_TYPE})

    if("${CMAKE_BUILD_TYPE}" STREQUAL "")
        set(CMAKE_BUILD_TYPE "Release")
    endif()
endif()

set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}")


add_subdirectory(src)

if(PROJECT_IS_TOP_LEVEL)
    add_subdirectory(tests)
endif()
