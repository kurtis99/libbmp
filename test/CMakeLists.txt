message ("Preparing tests")

file (GLOB SOURCES *.c)
message ("Files: ${SOURCES}")
foreach (testsourcefile ${SOURCES})
	get_filename_component (filename ${testsourcefile} NAME)
	string (REPLACE ".c" "" target ${filename})
	add_executable (${target} ${testsourcefile})
	target_link_libraries (${target} bmp)
	add_definitions (-DBMP_TEST_DIR=${CMAKE_CURRENT_SOURCE_DIR}/)
endforeach (testsourcefile ${SOURCES})

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output_test)
