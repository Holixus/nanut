cmake_minimum_required(VERSION 3.0)

option(ENABLE_EXAMPLES "Build nanut example programs" OFF)
option(ENABLE_TESTS "Build nanut test programs" OFF)

include_directories(include/)

add_subdirectory(library)
add_subdirectory(include)

if(ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif()

if(ENABLE_TESTS)
    add_subdirectory(tests)
endif()
