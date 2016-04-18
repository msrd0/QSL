cmake_minimum_required(VERSION 2.8.11)
project(QSL)

set(QSL_MAJOR 0)
set(QSL_MINOR 1)
set(QSL_PATCH 0)
set(QSL_VERSION ${QSL_MAJOR}.${QSL_MINOR}.${QSL_PATCH})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
configure_file(qsl_global.h.in qsl_global.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qsl_global.h
	    DESTINATION include/qsl)

find_package(Qt5Core REQUIRED)
find_package(Qt5Sql  REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
if(CMAKE_BUILD_TYPE MATCHES Debug)
	add_definitions("-DCMAKE_DEBUG")
endif()

add_subdirectory(core)
add_subdirectory(doc)
add_subdirectory(parser)
add_subdirectory(qslc)

configure_file(QSLConfig.cmake.in QSLConfig.cmake @ONLY)
configure_file(QSLMacros.cmake.in QSLMacros.cmake @ONLY)
configure_file(QSLVersion.cmake.in QSLVersion.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QSLVersion.cmake
	          ${CMAKE_CURRENT_BINARY_DIR}/QSLConfig.cmake
			  ${CMAKE_CURRENT_BINARY_DIR}/QSLMacros.cmake
	    DESTINATION lib/cmake/QSL)
