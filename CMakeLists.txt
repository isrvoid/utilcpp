cmake_minimum_required(VERSION 3.1.3 FATAL_ERROR)

project(Util)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(UtilFunctions)
include(CompilerFlags)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${CMAKE_SOURCE_DIR}/src)

add_subdirectory(src)

get_property(APP_SOURCES GLOBAL PROPERTY G_APP_SOURCES)

# unittest
# ------------------------------------------------------------------------------
enable_testing()
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
get_property(TEST_SOURCES GLOBAL PROPERTY G_TEST_SOURCES)

add_executable(unittest unittest.cpp ${APP_SOURCES} ${TEST_SOURCES})
target_compile_definitions(unittest PUBLIC UNITTEST)
target_link_libraries(unittest ${GTEST_LIBRARIES} pthread atomic)
add_test(NAME "${PROJECT_NAME}Test" COMMAND unittest)
