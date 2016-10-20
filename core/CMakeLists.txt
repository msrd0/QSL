set(QSLCORE_HEADERS
	qslcolumn.h
	qsldb.h
	qslfilter.h
	qslnamespace.h
	qslpassword.h
	qslquery.h
	qsltable.h
	qslvariant.h)
set(QSLCORE_SOURCES
	qsldb.cpp
	qslfilter.cpp
	qslpassword.cpp
	qslquery.cpp
	qslvariant.cpp)

add_subdirectory(driver)

add_library(qslcore SHARED
        ${QSLCORE_HEADERS}
		${QSLCORE_SOURCES}
    )
target_link_libraries(qslcore Qt5::Core Qt5::Sql qsld)
set_target_properties(qslcore PROPERTIES VERSION ${QSL_VERSION} SOVERSION ${QSL_MAJOR})

install(TARGETS qslcore
	    EXPORT QSLCoreConfig
	    RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib)
install(FILES ${QSLCORE_HEADERS}
	    DESTINATION include/qsl)
install(EXPORT QSLCoreConfig
	    NAMESPACE QSLCore_
		DESTINATION lib/cmake/QSLCore)
configure_file(../QSLVersion.cmake.in QSLCoreConfigVersion.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QSLCoreConfigVersion.cmake
	    DESTINATION lib/cmake/QSLCore)
