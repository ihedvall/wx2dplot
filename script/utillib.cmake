include (FetchContent)
include (CmakePrintHelpers)

FetchContent_Declare(utillib
        GIT_REPOSITORY https://github.com/ihedvall/utillib.git
        GIT_TAG HEAD)
set(UTIL_DOC OFF)
set(UTIL_TEST OFF)
set(UTIL_TOOLS OFF)
set(UTIL_LEX OFF)
#set(BUILD_SHARED_LIBS OFF)

FetchContent_MakeAvailable(utillib)

cmake_print_variables(utillib_POPULATED
        utillib_SOURCE_DIR
        utillib_BINARY_DIR)

cmake_print_properties(TARGETS util
        PROPERTIES INTERFACE_INCLUDE_DIRECTORIES
        INTERFACE_LINK_LIBRARIES)
