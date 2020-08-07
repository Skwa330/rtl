include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Core.cmake)

project(foslParser)

set(SOURCES ${CMAKE_CURRENT_LIST_DIR}/Parser/AST.cpp ${CMAKE_CURRENT_LIST_DIR}/Parser/Lexer.cpp ${CMAKE_CURRENT_LIST_DIR}/Parser/Parser.cpp)

if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_definitions(-DFMT_HEADER_ONLY)

add_library(foslParser ${SOURCES})
target_include_directories(foslParser PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include ${CMAKE_CURRENT_LIST_DIR}/../deps/fmt/include)
target_link_libraries(foslParser PRIVATE foslCore)