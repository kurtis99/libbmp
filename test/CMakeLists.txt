enable_testing ()

find_package (Check REQUIRED)

include_directories (${CHECK_INCLUDE_DIRS})

add_executable (test_bmp
	test_main.c
	test_bmp.c
)

target_link_libraries (test_bmp ${CHECK_LIBRARIES} bmp pthread rt)

add_definitions (-DBMP_TEST_DIR=${CMAKE_CURRENT_SOURCE_DIR}/)

add_custom_command (
	TARGET test_bmp POST_BUILD
	COMMENT "Running unit tests"
	COMMAND test_bmp
)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/output_test)
