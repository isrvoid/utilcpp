cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(Util)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(CompilerFlags)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

if(${UNITTEST})
	enable_testing()
	find_package(GTest REQUIRED)
	include_directories(${GTEST_INCLUDE_DIRS})
endif()

add_subdirectory(src)
