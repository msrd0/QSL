set(SPISC_HEADERS
	generator.h)
set(SPISC_SOURCES
	generator.cpp
	main.cpp)
add_executable(spisc ${SPISC_HEADERS} ${SPISC_SOURCES})
target_link_libraries(spisc spisparser Qt5::Core)

install(TARGETS spisc
	    EXPORT SPISCConfig
	    RUNTIME DESTINATION ${BINDIR}
		LIBRARY DESTINATION ${LIBDIR}
		ARCHIVE DESTINATION ${LIBDIR})
install(EXPORT SPISCConfig
	    NAMESPACE SPISC_
	    DESTINATION ${CMAKEDIR}/SPISC)
configure_file(../cmake/SPISVersion.cmake.in SPISCConfigVersion.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SPISCConfigVersion.cmake
	    DESTINATION ${CMAKEDIR}/SPISC)
