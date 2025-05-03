cmake_minimum_required(VERSION 3.24)

project(tape-sorter CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -Werror -pedantic)
endif()

add_subdirectory(core)

option(ENABLE_TESTS "Build tests" OFF)
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND ENABLE_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}-core)
