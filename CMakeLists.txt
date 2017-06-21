cmake_minimum_required(VERSION 3.8)
project(cpp_utility)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp third-party/protobuf/Person.pb.cc include/common_tools.h)

include_directories(include)
include_directories(third-party/protobuf)

LINK_DIRECTORIES(/user/local/lib)

add_executable(cpp_utility ${SOURCE_FILES})

find_package(Threads REQUIRED)

target_link_libraries(cpp_utility protobuf)

target_link_libraries(cpp_utility ${CMAKE_THREAD_LIBS_INIT})