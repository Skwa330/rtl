include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Core.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Parser.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Sema.cmake)

project(rtlCompiler)

set(SOURCES Dump.cpp)
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/Compiler/)

if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_definitions(-DFMT_HEADER_ONLY)

add_library(rtlCompiler ${SOURCES})
target_include_directories(rtlCompiler PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include ${CMAKE_CURRENT_LIST_DIR}/../deps/fmt/include)
target_link_libraries(rtlCompiler PRIVATE rtlCore rtlParser rtlSema)
add_executable(rtl ${CMAKE_CURRENT_LIST_DIR}/Compiler/Main.cpp)
target_include_directories(rtl PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include ${CMAKE_CURRENT_LIST_DIR}/../deps/ya_getopt ${CMAKE_CURRENT_LIST_DIR}/../deps/fmt/include)
target_link_libraries(rtl PRIVATE ya_getopt rtlCompiler)
