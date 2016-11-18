set(QSLD_HEADERS
	db.h
	diff.h
	driver.h)
set(QSLD_SOURCES
	db.cpp
	diff.cpp
	driver.cpp)
add_library(qsld ${QSL_LIB_TYPE} ${QSLD_HEADERS} ${QSLD_SOURCES})
target_link_libraries(qsld ${CMAKE_DL_LIBS} Qt5::Core Qt5::Sql)
set_target_properties(qsld PROPERTIES VERSION ${QSL_VERSION} SOVERSION ${QSL_MAJOR})

install(TARGETS qsld
	    EXPORT QSLDriverConfig
		RUNTIME DESTINATION ${BINDIR}
		LIBRARY DESTINATION ${LIBDIR}
		ARCHIVE DESTINATION ${LIBDIR})
install(FILES ${QSLD_HEADERS}
	    DESTINATION include/qsl/driver)
install(EXPORT QSLDriverConfig
	    DESTINATION ${CMAKEDIR}/QSLDriver)
configure_file(../../QSLVersion.cmake.in QSLDriverConfigVersion.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QSLDriverConfigVersion.cmake
	    DESTINATION ${CMAKEDIR}/QSLDriver)

add_subdirectory(sqlite)
