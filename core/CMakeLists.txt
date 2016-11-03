set(QSLCORE_HEADERS
	pk.h
	qslcolumn.h
	qsldb.h
	qslfilter.h
	qslfilter_p.h
	qslnamespace.h
	qslquery.h
	qsltable.h)
set(QSLCORE_SOURCES
	qsldb.cpp
	qslfilter.cpp
	qslquery.cpp)
set(QSLVARIANT_HEADERS
	qslpassword.h
	qslvariant.h)
set(QSLVARIANT_SOURCES
	qslpassword.cpp
	qslvariant.cpp)

add_subdirectory(driver)

add_library(qslvariant SHARED
		${QSLVARIANT_HEADERS}
		${QSLVARIANT_SOURCES}
	)
target_link_libraries(qslvariant Qt5::Core)
set_target_properties(qslvariant PROPERTIES VERSION ${QSL_VERSION} SOVERSION ${QSL_MAJOR})
add_library(qslcore SHARED
        ${QSLCORE_HEADERS}
		${QSLCORE_SOURCES}
    )
target_link_libraries(qslcore Qt5::Core Qt5::Sql qsld qslvariant)
set_target_properties(qslcore PROPERTIES VERSION ${QSL_VERSION} SOVERSION ${QSL_MAJOR})

install(TARGETS qslcore qslvariant
	    EXPORT QSLCoreConfig
	    RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib)
install(FILES ${QSLCORE_HEADERS} ${QSLVARIANT_HEADERS}
	    DESTINATION include/qsl)
install(EXPORT QSLCoreConfig
	    NAMESPACE QSLCore_
		DESTINATION lib/cmake/QSLCore)
configure_file(../QSLVersion.cmake.in QSLCoreConfigVersion.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QSLCoreConfigVersion.cmake
	    DESTINATION lib/cmake/QSLCore)
