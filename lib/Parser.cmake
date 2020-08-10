include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Core.cmake)

project(rltParser)

set(SOURCES AST.cpp Lexer.cpp Parser.cpp)

list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/Parser/)

if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_definitions(-DFMT_HEADER_ONLY)

add_library(rltParser ${SOURCES})
target_include_directories(rltParser PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include ${CMAKE_CURRENT_LIST_DIR}/../deps/fmt/include)
target_link_libraries(rltParser PRIVATE rltCore)