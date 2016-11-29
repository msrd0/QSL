cmake_minimum_required(VERSION 2.8.11)
project(SPIS-Example-Authenticator)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

find_package(SPIS REQUIRED)
include_directories(${SPIS_INCLUDE_DIRS})
add_executable(auth main.cpp)
spis_compile(TARGET auth FILES auth.spis)
