cmake_minimum_required (VERSION 2.8)
project (libbmp)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set (CMAKE_C_FLAGS_RELEASE "-DNDEBUG -Os")
set (CMAKE_C_FLAGS_DEBUG "-DDEBUG -O0 -ggdb")

add_subdirectory (src)
add_subdirectory (test)
