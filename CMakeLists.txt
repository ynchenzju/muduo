cmake_minimum_required(VERSION 3.16)
project(remote_project)

set(CMAKE_CXX_STANDARD 14)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

include_directories(${PROJECT_SOURCE_DIR}/include)
aux_source_directory(src SRC_DIR)
add_executable(remote_project ${SRC_DIR} main.cpp)
target_link_libraries(remote_project Threads::Threads)
