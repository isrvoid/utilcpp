set_property(GLOBAL PROPERTY G_APP_SOURCES)
set_property(GLOBAL PROPERTY G_TEST_SOURCES)

# relative paths only
function(_add_files_to_property PROPERTY)
	foreach (_file ${ARGN})
		set_property(GLOBAL APPEND PROPERTY ${PROPERTY} ${CMAKE_CURRENT_LIST_DIR}/${_file})
	endforeach()
endfunction()

function(add_sources)
	_add_files_to_property(G_APP_SOURCES ${ARGN})
endfunction()

function(add_test_sources)
	_add_files_to_property(G_TEST_SOURCES ${ARGN})
endfunction()
