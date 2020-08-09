include_guard()

project(ya_getopt)

set(SOURCES ${CMAKE_CURRENT_LIST_DIR}/deps/ya_getopt/ya_getopt.c)

add_library(ya_getopt ${SOURCES})