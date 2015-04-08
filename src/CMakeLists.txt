set (SRCS
	bmp.c
	debug.c
	file_io.c
)

add_library (bmp ${SRCS})

install (FILES ${CMAKE_SOURCE_DIR}/src/bmp.h DESTINATION include/libbmp)
