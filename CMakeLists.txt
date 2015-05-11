cmake_minimum_required (VERSION 2.8)
project (libbmp)

set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra")
set (CMAKE_C_FLAGS_RELEASE "-DNDEBUG -Os -Werror")
set (CMAKE_C_FLAGS_DEBUG "-DDEBUG -O0 -ggdb")

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/CMakeModules/")

add_subdirectory (src)
add_subdirectory (test)
