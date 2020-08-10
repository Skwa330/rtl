include_guard()

project(rltCore)

set(SOURCES Error.cpp SourceLocation.cpp Timing.cpp)
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/Core/)

if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_definitions(-DFMT_HEADER_ONLY)

add_library(rltCore ${SOURCES})
target_include_directories(rltCore PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include ${CMAKE_CURRENT_LIST_DIR}/../deps/fmt/include)