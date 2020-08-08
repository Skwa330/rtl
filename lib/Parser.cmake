include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Core.cmake)

project(langParser)

set(SOURCES AST.cpp Error.cpp Lexer.cpp Parser.cpp SourceLocation.cpp)

list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/Parser/)

if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_definitions(-DFMT_HEADER_ONLY)

add_library(langParser ${SOURCES})
target_include_directories(langParser PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include ${CMAKE_CURRENT_LIST_DIR}/../deps/fmt/include)
target_link_libraries(langParser PRIVATE langCore)