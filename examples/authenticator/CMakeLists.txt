cmake_minimum_required(VERSION 2.8.11)
project(QSL-Example-Authenticator)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(QSL REQUIRED)
include_directories(${QSL_INCLUDE_DIRS})
add_executable(auth main.cpp)
qsl_compile(TARGET auth FILES auth.qsl)
