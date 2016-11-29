set(SPISDSQLITE_HEADERS
	sqlitedb.h
	sqlitedriver.h
	sqlitetypes.h)
set(SPISDSQLITE_SOURCES
	sqlite.cpp
	sqlitedb.cpp
	sqlitedriver.cpp
	sqlitetypes.cpp)
add_library(spisdsqlite SHARED ${SPISDSQLITE_HEADERS} ${SPISDSQLITE_SOURCES})
target_link_libraries(spisdsqlite spisd spisvariant)
set_target_properties(spisdsqlite PROPERTIES VERSION ${SPIS_VERSION} SOVERSION ${SPIS_MAJOR})

install(TARGETS spisdsqlite
	    RUNTIME DESTINATION ${BINDIR}
		LIBRARY DESTINATION ${LIBDIR})
