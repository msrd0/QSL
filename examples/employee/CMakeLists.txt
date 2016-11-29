cmake_minimum_required(VERSION 2.8.11)
project(SPIS-Example-Employee)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(SPIS REQUIRED)
add_executable(employee main.cpp)
spis_compile(TARGET employee FILES employee.spis)
