include_guard()

project(langCore)

set(SOURCES ${CMAKE_CURRENT_LIST_DIR}/Core/Timing.cpp)

if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_definitions(-DFMT_HEADER_ONLY)

add_library(langCore ${SOURCES})
target_include_directories(langCore PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include ${CMAKE_CURRENT_LIST_DIR}/../deps/fmt/include)