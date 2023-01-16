cmake_minimum_required(VERSION 3.20)
project(muduo)

set(CMAKE_CXX_STANDARD 14)
include_directories(src)
aux_source_directory(src SRC_DIR)

add_executable(muduo ${SRC_DIR})
