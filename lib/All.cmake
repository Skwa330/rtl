include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Compiler.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Core.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Parser.cmake)

set(FOSL_ALL_LIBS foslCore foslParser)