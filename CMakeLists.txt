cmake_minimum_required(VERSION 2.6)
project(tinyrpc)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

if(NOT CMAKE_CODE_FORMATTER)
    set(CMAKE_CODE_FORMATTER echo)
endif()

set(CXX_FLAGS
        -fno-omit-frame-pointer 
        -Wall
        -Wextra
        -Werror
        -Wconversion
        -Wno-unused-parameter
        -Wold-style-cast
        -Woverloaded-virtual
        -Wpointer-arith
        -Wshadow
        -Wwrite-strings
        -std=c++17
        -march=native
        -rdynamic)
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(
        3rd/tinynet
        3rd/rapidjson/include/
        3rd/doctest/doctest
        ${PROJECT_SOURCE_DIR}
        ${PROJECT_BINARY_DIR})

add_subdirectory(3rd/tinynet)
add_subdirectory(tinyrpc)



if(NOT CMAKE_BUILD_NO_EXAMPLES)
    add_subdirectory(example)
endif()

# if(NOT CMAKE_BUILD_NO_TESTS)
#     add_subdirectory(tests)
# endif()