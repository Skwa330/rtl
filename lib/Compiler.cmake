include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Core.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Parser.cmake)

project(rltCompiler)

set(SOURCES Dump.cpp)
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/Compiler/)

if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_definitions(-DFMT_HEADER_ONLY)

add_library(rltCompiler ${SOURCES})
target_include_directories(rltCompiler PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include ${CMAKE_CURRENT_LIST_DIR}/../deps/ya_getopt ${CMAKE_CURRENT_LIST_DIR}/../deps/filesystem/include ${CMAKE_CURRENT_LIST_DIR}/../deps/fmt/include)
target_link_libraries(rltCompiler PRIVATE rltCore rltParser)
add_executable(rlt ${CMAKE_CURRENT_LIST_DIR}/Compiler/Main.cpp)
target_include_directories(rlt PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include ${CMAKE_CURRENT_LIST_DIR}/../deps/ya_getopt ${CMAKE_CURRENT_LIST_DIR}/../deps/filesystem/include ${CMAKE_CURRENT_LIST_DIR}/../deps/fmt/include)
target_link_libraries(rlt PRIVATE ya_getopt rltCompiler)
