set(SPISD_HEADERS
	db.h
	diff.h
	driver.h)
set(SPISD_SOURCES
	db.cpp
	diff.cpp
	driver.cpp)
add_library(spisd ${SPIS_LIB_TYPE} ${SPISD_HEADERS} ${SPISD_SOURCES})
target_link_libraries(spisd ${CMAKE_DL_LIBS} Qt5::Core Qt5::Sql)
set_target_properties(spisd PROPERTIES VERSION ${SPIS_VERSION} SOVERSION ${SPIS_MAJOR})

install(TARGETS spisd
	    EXPORT SPISDriverConfig
		RUNTIME DESTINATION ${BINDIR}
		LIBRARY DESTINATION ${LIBDIR}
		ARCHIVE DESTINATION ${LIBDIR})
install(FILES ${SPISD_HEADERS}
	    DESTINATION include/spis/driver)
install(EXPORT SPISDriverConfig
	    DESTINATION ${CMAKEDIR}/SPISDriver)
configure_file(../../cmake/SPISVersion.cmake.in SPISDriverConfigVersion.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SPISDriverConfigVersion.cmake
	    DESTINATION ${CMAKEDIR}/SPISDriver)

add_subdirectory(mysql)
add_subdirectory(sqlite)
