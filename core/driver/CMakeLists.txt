set(QSLD_HEADERS
	db.h
	diff.h
	driver.h)
set(QSLD_SOURCES
	db.cpp
	diff.cpp
	driver.cpp)
add_library(qsld SHARED ${QSLD_HEADERS} ${QSLD_SOURCES})
target_link_libraries(qsld ${CMAKE_DL_LIBS} Qt5::Core Qt5::Sql)
set_target_properties(qsld PROPERTIES VERSION ${QSL_VERSION} SOVERSION ${QSL_MAJOR})

install(TARGETS qsld
	    EXPORT QSLDriverConfig
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib)
install(FILES ${QSLD_HEADERS}
	    DESTINATION include/qsl/driver)
install(EXPORT QSLDriverConfig
	    DESTINATION lib/cmake/QSLDriver)
configure_file(../../QSLVersion.cmake.in QSLDriverConfigVersion.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QSLDriverConfigVersion.cmake
	    DESTINATION lib/cmake/QSLDriver)

add_subdirectory(psql)
add_subdirectory(sqlite)
