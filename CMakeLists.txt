cmake_minimum_required(VERSION 3.6)
project(cpp_utility)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp base_tool/block_queue.h)
add_executable(cpp_utility ${SOURCE_FILES})