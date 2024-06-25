cmake_minimum_required(VERSION 3.14)

set(LIBARY_NAME my-vector)

add_library(${LIBARY_NAME} INTERFACE custom_vector.hpp)
target_include_directories(${LIBARY_NAME} INTERFACE ./)