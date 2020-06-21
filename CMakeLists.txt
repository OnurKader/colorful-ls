cmake_minimum_required(VERSION 3.4)

project(List LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(List main.cpp)

find_package(fmt)
target_link_libraries(List fmt::fmt)
