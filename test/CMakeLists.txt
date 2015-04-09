enable_testing ()

message ("Preparing tests")

find_package (Check REQUIRED)

include_directories (${CHECK_INCLUDE_DIRS})

file (GLOB SOURCES *.c)
message ("Files: ${SOURCES}")
foreach (testsourcefile ${SOURCES})
	get_filename_component (filename ${testsourcefile} NAME)
	string (REPLACE ".c" "" target ${filename})
	add_executable (${target} ${testsourcefile})
	target_link_libraries (${target} ${CHECK_LIBRARIES} bmp pthread)
	add_definitions (-DBMP_TEST_DIR=${CMAKE_CURRENT_SOURCE_DIR}/)
	add_test(NAME ${target}
		WORKING_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}
		COMMAND ${target})
endforeach (testsourcefile ${SOURCES})

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output_test)
