include_guard()

project(foslCore)

set(SOURCES ${CMAKE_CURRENT_LIST_DIR}/Core/Error.cpp)

if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_definitions(-DFMT_HEADER_ONLY)

add_library(foslCore ${SOURCES})
target_include_directories(foslCore PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include ${CMAKE_CURRENT_LIST_DIR}/../deps/fmt/include)