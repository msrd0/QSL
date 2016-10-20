execute_process(COMMAND pkg-config --cflags Qt5Core Qt5Sql
	            OUTPUT_VARIABLE QT_CFLAGS
				OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" QT_CFLAGS ${QT_CFLAGS})
execute_process(COMMAND pkg-config --libs Qt5Core Qt5Sql
	            OUTPUT_VARIABLE QT_LDFLAGS
				OUTPUT_STRIP_TRAILING_WHITESPACE)
string(REPLACE " " ";" QT_LDFLAGS ${QT_LDFLAGS})

add_subdirectory(qslc)
