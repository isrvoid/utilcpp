include(UtilFunctions)

add_subdirectory(util)

get_property(APP_SOURCES GLOBAL PROPERTY G_APP_SOURCES)

if(${UNITTEST})
	get_property(TEST_SOURCES GLOBAL PROPERTY G_TEST_SOURCES)
	add_executable(unittest unittest.cpp ${APP_SOURCES} ${TEST_SOURCES})
	target_link_libraries(unittest ${GTEST_LIBRARIES})
	target_link_libraries(unittest pthread)
	add_test(NAME "${PROJECT_NAME}Test" COMMAND unittest)
endif()
