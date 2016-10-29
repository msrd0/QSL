find_package(PkgConfig REQUIRED)
execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --cflags Qt5Core Qt5Sql
	            OUTPUT_VARIABLE QT_CFLAGS
				OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" QT_CFLAGS ${QT_CFLAGS})
execute_process(COMMAND ${PKG_CONFIG_EXECUTABLE} --libs Qt5Core Qt5Sql
	            OUTPUT_VARIABLE QT_LDFLAGS
				OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" QT_LDFLAGS ${QT_LDFLAGS})

set(TEST_CFLAGS
	-isystem ${CMAKE_CURRENT_BINARY_DIR}/../
    -isystem ${CMAKE_CURRENT_SOURCE_DIR}/../core/
	${QT_CFLAGS})
set(TEST_LDFLAGS
	-L${CMAKE_CURRENT_BINARY_DIR}/../core/
    -L${CMAKE_CURRENT_BINARY_DIR}/../core/driver/
    -lqsld -lqslcore
	${QT_LDFLAGS})

add_subdirectory(qslc)
add_subdirectory(qsldump)
