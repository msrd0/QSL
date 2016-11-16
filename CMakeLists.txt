cmake_minimum_required(VERSION 3.1)
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

find_package(Qt5 5.5.0 REQUIRED COMPONENTS Core Sql)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Werror=format -Werror=return-type")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=format -Werror=return-type")

if(CMAKE_BUILD_TYPE MATCHES Debug)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -fsanitize=undefined")
	add_definitions("-D_GLIBCXX_DEBUG")
	add_definitions("-DQT_SHAREDPOINTER_TRACK_POINTERS")
	add_definitions("-DCMAKE_DEBUG")
endif()
add_definitions("-DCMAKE_QSL_SO")

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core)

add_subdirectory(core)
add_subdirectory(doc)
add_subdirectory(parser)
add_subdirectory(qslc)
add_subdirectory(qsldump)

configure_file(QSLConfig.cmake.in QSLConfig.cmake @ONLY)
configure_file(QSLMacros.cmake.in QSLMacros.cmake @ONLY)
configure_file(QSLVersion.cmake.in QSLVersion.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QSLVersion.cmake
	          ${CMAKE_CURRENT_BINARY_DIR}/QSLConfig.cmake
			  ${CMAKE_CURRENT_BINARY_DIR}/QSLMacros.cmake
	    DESTINATION lib/cmake/QSL)
configure_file(qt_QSL.pri.in qt_QSL.pri @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qt_QSL.pri
	    DESTINATION /usr/lib/qt/mkspecs/modules)

if (NOT NO_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
