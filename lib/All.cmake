include_guard()

include(${CMAKE_CURRENT_LIST_DIR}/Compiler.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Core.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Parser.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/Sema.cmake)

set(RTL_ALL_LIBS rtlCompiler rtlCore rtlParser rtlSema)