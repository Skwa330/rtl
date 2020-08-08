include_guard()

cmake_minimum_required(VERSION 3.0)

include(${CMAKE_CURRENT_LIST_DIR}/BuildDependencies.cmake)

set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_CXX_STANDARD 17)

include(${CMAKE_CURRENT_LIST_DIR}/lib/All.cmake)

project(foslLib)

file(GLOB_RECURSE SOURCES source/fosl/*.cpp)

if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_definitions(-DFMT_HEADER_ONLY)

file(WRITE ${CMAKE_CURRENT_LIST_DIR}/null/null "")
add_library(foslLib ${CMAKE_CURRENT_LIST_DIR}/null/null)
target_link_libraries(foslLib PUBLIC ${FOSL_ALL_LIBS})
