cmake_minimum_required(VERSION 3.1)
project(SPIS)

option(BUILD_SHARED_LIBS "Build shared libraries rather than static ones" ON)
if (BUILD_SHARED_LIBS)
	set(SPIS_LIB_TYPE SHARED)
else()
	set(SPIS_LIB_TYPE STATIC)
endif()

option(SQLITE_NO_ROWID "The SQLite driver doesn't support WITHOUT ROWID tables" OFF)
if (SQLITE_NO_ROWID)
	add_definitions(-DSPIS_SQLITE_NO_ROWID)
endif()

set(LIBDIR "lib/" CACHE STRING "Installation path of the libraries")
set(BINDIR "bin/" CACHE STRING "Installation path of the binaries")
set(CMAKEDIR "lib/cmake" CACHE STRING "Installation path of the cmake config")
set(QMAKEDIR "/usr/lib/qt/mkspecs" CACHE STRING "Installation path of the qmake mkspecs")
set(KATEPART4DIR "/usr/share/apps/katepart" CACHE STRING "Data path of KatePart4 to install syntax highlighting files")
set(KATEPART5DIR "/usr/share/katepart5" CACHE STRING "Data path of KatePart5 to install syntax highlighting files")

set(SPIS_MAJOR 0)
set(SPIS_MINOR 1)
set(SPIS_PATCH 2)
set(SPIS_VERSION ${SPIS_MAJOR}.${SPIS_MINOR}.${SPIS_PATCH})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
include_directories(${CMAKE_CURRENT_BINARY_DIR})
configure_file(spis_global.h.in spis_global.h)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/spis_global.h
	    DESTINATION include/spis)

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
if (BUILD_SHARED_LIBS)
	add_definitions("-DCMAKE_SPIS_SO")
else()
	add_definitions("-DCMAKE_SPIS_A")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/core)

add_subdirectory(core)
add_subdirectory(doc)
add_subdirectory(highlight)
add_subdirectory(parser)
add_subdirectory(spisc)
add_subdirectory(spisdump)

configure_file(cmake/SPISConfig.cmake.in SPISConfig.cmake @ONLY)
configure_file(cmake/SPISMacros.cmake.in SPISMacros.cmake @ONLY)
configure_file(cmake/SPISVersion.cmake.in SPISVersion.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SPISVersion.cmake
	          ${CMAKE_CURRENT_BINARY_DIR}/SPISConfig.cmake
			  ${CMAKE_CURRENT_BINARY_DIR}/SPISMacros.cmake
	    DESTINATION ${CMAKEDIR}/SPIS)
configure_file(qt_SPIS.pri.in qt_SPIS.pri @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qt_SPIS.pri
	    DESTINATION ${QMAKEDIR}/modules)

if (NOT NO_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
