set(QSLDSQLITE_HEADERS
	sqlitedb.h
	sqlitedriver.h
	sqlitetypes.h)
set(QSLDSQLITE_SOURCES
	sqlite.cpp
	sqlitedb.cpp
	sqlitedriver.cpp
	sqlitetypes.cpp)
add_library(qsldsqlite SHARED ${QSLDSQLITE_HEADERS} ${QSLDSQLITE_SOURCES})
target_link_libraries(qsldsqlite qsld)
set_target_properties(qsldsqlite PROPERTIES VERSION ${QSL_VERSION} SOVERSION ${QSL_MAJOR})

install(TARGETS qsldsqlite
	    RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib)
