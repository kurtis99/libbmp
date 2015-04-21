set (SRCS
	bmp.c
	debug.c
	file_io.c
)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fprofile-arcs -ftest-coverage")

add_executable (dumper dumper.c)
add_library (bmp ${SRCS})
target_link_libraries (bmp gcov)
target_link_libraries (dumper bmp)

install (FILES ${CMAKE_SOURCE_DIR}/src/bmp.h DESTINATION include/libbmp)
