cmake_minimum_required(VERSION 3.10)

set (CMAKE_CXX_FLAGS -std=c++20)
SET(CMAKE_EXPORT_COMPILE_COMMANDS ON) # for clangd
set(CMAKE_CXX_STANDARD 20)  # compile as C++20
set(CMAKE_CXX_STANDARD_REQUIRED 20) # require C++20 support

project(cpplox)

message(STATUS "CMAKE_CXX_FLAGS: ${CMAKE_CXX_FLAGS}")
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: ${CMAKE_FXX_FLAGS_DEBUG}")

# includes
set(CPPLOX_SRC_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

include_directories(${CPPLOX_SRC_INCLUDE_DIR})

add_library(libcpplox STATIC src/main.cpp)

add_executable(cpplox src/main.cpp)
