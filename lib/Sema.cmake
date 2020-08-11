include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Core.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Parser.cmake)

project(rtlSema)

set(SOURCES Driver.cpp Type.cpp Validator.cpp)
list(TRANSFORM SOURCES PREPEND ${CMAKE_CURRENT_LIST_DIR}/Sema/)

if (WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_definitions(-DFMT_HEADER_ONLY)

add_library(rtlSema ${SOURCES})
target_include_directories(rtlSema PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../include ${CMAKE_CURRENT_LIST_DIR}/../deps/fmt/include)
target_link_libraries(rtlSema PRIVATE rtlCore rtlParser)
