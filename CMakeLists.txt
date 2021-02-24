cmake_minimum_required(VERSION 3.13)

project(1d-minimize)

set(CMAKE_CXX_STANDARD 17)

include_directories("${CMAKE_SOURCE_DIR}/headers")

file(GLOB_RECURSE src "${CMAKE_SOURCE_DIR}/src/*.cpp")

add_executable(1d-minimize "${src}")
