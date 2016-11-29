set(SPISCORE_HEADERS
	pk.h
	pk_p.h
	spiscolumn.h
	spisdb.h
	spisfilter.h
	spisfilter_p.h
	spisnamespace.h
	spisquery.h
	spistable.h)
set(SPISCORE_SOURCES
	spisdb.cpp
	spisfilter.cpp)
set(SPISVARIANT_HEADERS
	spispassword.h
	spisvariant.h)
set(SPISVARIANT_SOURCES
	spispassword.cpp
	spisvariant.cpp)

add_subdirectory(driver)

add_library(spisvariant ${SPIS_LIB_TYPE}
		${SPISVARIANT_HEADERS}
		${SPISVARIANT_SOURCES}
	)
target_link_libraries(spisvariant Qt5::Core)
set_target_properties(spisvariant PROPERTIES VERSION ${SPIS_VERSION} SOVERSION ${SPIS_MAJOR})
add_library(spiscore ${SPIS_LIB_TYPE}
        ${SPISCORE_HEADERS}
		${SPISCORE_SOURCES}
    )
target_link_libraries(spiscore Qt5::Core Qt5::Sql spisd spisvariant)
set_target_properties(spiscore PROPERTIES VERSION ${SPIS_VERSION} SOVERSION ${SPIS_MAJOR})

install(TARGETS spiscore spisvariant
	    EXPORT SPISCoreConfig
	    RUNTIME DESTINATION ${BINDIR}
		LIBRARY DESTINATION ${LIBDIR}
		ARCHIVE DESTINATION ${LIBDIR})
install(FILES ${SPISCORE_HEADERS} ${SPISVARIANT_HEADERS}
	    DESTINATION include/spis)
install(EXPORT SPISCoreConfig
	    NAMESPACE SPISCore_
		DESTINATION ${CMAKEDIR}/SPISCore)
configure_file(../cmake/SPISVersion.cmake.in SPISCoreConfigVersion.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SPISCoreConfigVersion.cmake
	    DESTINATION ${CMAKEDIR}/SPISCore)
