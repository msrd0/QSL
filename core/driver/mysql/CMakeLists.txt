set(SPISDMYSQL_HEADERS
	mysqldb.h
	mysqldriver.h
	mysqltypes.h)
set(SPISDMYSQL_SOURCES
	mysql.cpp
	mysqldb.cpp
	mysqldriver.cpp
	mysqltypes.cpp)
add_library(spisdmysql SHARED ${SPISDMYSQL_HEADERS} ${SPISDMYSQL_SOURCES})
target_link_libraries(spisdmysql spisd spisvariant)
set_target_properties(spisdmysql PROPERTIES VERSION ${SPIS_VERSION} SOVERSION ${SPIS_MAJOR})

install(TARGETS spisdmysql
        RUNTIME DESTINATION ${BINDIR}
        LIBRARY DESTINATION ${LIBDIR})
