cmake_minimum_required(VERSION 2.8.11)
project(QSL-Example-Simple)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(QSL REQUIRED)
include_directories(${QSL_INCLUDE_DIRS})
add_executable(simple main.cpp)
qsl_compile(TARGET simple FILES example.qsl)
