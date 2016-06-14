cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

project(Util)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(CompilerFlags)

include_directories(${CMAKE_SOURCE_DIR}/src)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include(UtilFunctions)

add_subdirectory(src)

get_property(APP_SOURCES GLOBAL PROPERTY G_APP_SOURCES)

if(UNITTEST)
	enable_testing()
	find_package(GTest REQUIRED)
	include_directories(${GTEST_INCLUDE_DIRS})

	get_property(TEST_SOURCES GLOBAL PROPERTY G_TEST_SOURCES)
	add_executable(unittest unittest.cpp ${APP_SOURCES} ${TEST_SOURCES})
	target_link_libraries(unittest ${GTEST_LIBRARIES})
	target_link_libraries(unittest pthread)
	add_test(NAME "${PROJECT_NAME}Test" COMMAND unittest)
endif()
