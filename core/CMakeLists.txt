include_directories(include)
set(QSLCORE_HEADERS include/qsldb.h include/qslfilter.h include/qslquery.h include/qsltable.h)
set(QSLCORE_SOURCES     src/qsldb.cpp   src/qslfilter.cpp   src/qslquery.cpp   src/qsltable.cpp)
qt5_add_resources(QSLCORE_RCC drivers/drivers.qrc)
add_library(qslcore SHARED
        ${QSLCORE_HEADERS}
		${QSLCORE_SOURCES}
		${QSLCORE_RCC}
    )
target_link_libraries(qslcore Qt5::Core Qt5::Sql)
set_target_properties(qslcore PROPERTIES VERSION ${QSL_VERSION} SOVERSION ${QSL_MAJOR})

install(TARGETS qslcore
	    EXPORT QSLCoreConfig
	    RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		PUBLIC_HEADER DESTINATION include/qsl)
install(FILES ${QSLCORE_HEADERS}
	    DESTINATION include/qsl)
install(EXPORT QSLCoreConfig
	    NAMESPACE QSLCore_
		DESTINATION lib/cmake/QSLCore)
configure_file(../QSLVersion.cmake.in QSLCoreConfigVersion.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QSLCoreConfigVersion.cmake
	    DESTINATION lib/cmake/QSLCore)

find_package(Doxygen)
if (DOXYGEN_FOUND)
	configure_file(../doc/Doxyfile.in Doxyfile)
	add_custom_target(doc
		COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating Documentation using Doxygen"
		VERBATIM)
	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/doc/html DESTINATION share/doc/qsl)
else()
	message("Doxygen not found, won't build documentation")
endif()
