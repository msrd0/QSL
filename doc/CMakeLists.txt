find_package(Doxygen)
if (DOXYGEN_FOUND)
	configure_file(../doc/Doxyfile.in Doxyfile)
	add_custom_target(doc
		COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
		COMMENT "Generating Documentation using Doxygen"
		VERBATIM)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION share/doc/qsl)
else()
	message("Doxygen not found, won't build documentation")
endif()
