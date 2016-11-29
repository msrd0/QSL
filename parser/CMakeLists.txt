set(SPISPARSER_HEADERS
	database.h
	parser.h
	table.h)
set(SPISPARSER_SOURCES
	database.cpp
	parser.cpp
	table.cpp)
add_library(spisparser ${SPIS_LIB_TYPE} ${SPISPARSER_HEADERS} ${SPISPARSER_SOURCES})
target_link_libraries(spisparser Qt5::Core Qt5::Sql spiscore)
set_target_properties(spisparser PROPERTIES VERSION ${SPIS_VERSION} SOVERSION ${SPIS_MAJOR})

install(TARGETS spisparser
		EXPORT SPISParserConfig
		RUNTIME DESTINATION ${BINDIR}
		LIBRARY DESTINATION ${LIBDIR}
		ARCHIVE DESTINATION ${LIBDIR})
install(FILES ${SPISPARSER_HEADERS}
		DESTINATION include/spis/parser)
install(EXPORT SPISParserConfig
		DESTINATION ${CMAKEDIR}/SPISParser)
configure_file(../cmake/SPISVersion.cmake.in SPISParserConfigVersion.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SPISParserConfigVersion.cmake
		DESTINATION ${CMAKEDIR}/SPISParser)
