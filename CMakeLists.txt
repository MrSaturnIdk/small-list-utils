cmake_minimum_required(VERSION 3.12)
project(SmallListUtils
	VERSION 1.0.0
	DESCRIPTION "An extremely tiny C99 project for parsing a file with items separated by newlines"
	HOMEPAGE_URL "https://github.com/MrSaturnIdk/small-list-utils"
	LANGUAGES C
)


if(NOT CMAKE_BUILD_TYPE AND "${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
	set(CMAKE_BUILD_TYPE Release CACHE STRING "CMake build type (what do you think)" FORCE)
endif()
if("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
	message(STATUS "Project version is: ${PROJECT_VERSION}")
	message(STATUS "CMAKE_BUILD_TYPE is set to: ${CMAKE_BUILD_TYPE}")
endif()

add_library(slu STATIC
	src/list.c
)
target_include_directories(slu PUBLIC include)
target_include_directories(slu PRIVATE src/utils)
set_target_properties(slu PROPERTIES
	C_STANDARD 99
	C_STANDARD_REQUIRED ON
	C_EXTENSIONS OFF
)
