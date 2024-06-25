cmake_minimum_required(VERSION 3.14)

project(vector-cpp LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -O0)

add_subdirectory(source)
add_subdirectory(vector)