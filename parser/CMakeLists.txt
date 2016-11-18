set(QSLPARSER_HEADERS
	database.h
	parser.h
	table.h)
set(QSLPARSER_SOURCES
	database.cpp
	parser.cpp
	table.cpp)
add_library(qslparser ${QSL_LIB_TYPE} ${QSLPARSER_HEADERS} ${QSLPARSER_SOURCES})
target_link_libraries(qslparser Qt5::Core Qt5::Sql qslcore)
set_target_properties(qslparser PROPERTIES VERSION ${QSL_VERSION} SOVERSION ${QSL_MAJOR})

install(TARGETS qslparser
		EXPORT QSLParserConfig
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
install(FILES ${QSLPARSER_HEADERS}
		DESTINATION include/qsl/parser)
install(EXPORT QSLParserConfig
		DESTINATION lib/cmake/QSLParser)
configure_file(../QSLVersion.cmake.in QSLParserConfigVersion.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QSLParserConfigVersion.cmake
		DESTINATION lib/cmake/QSLParser)
