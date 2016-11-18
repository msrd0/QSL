set(QSLC_HEADERS
	generator.h)
set(QSLC_SOURCES
	generator.cpp
	main.cpp)
add_executable(qslc ${QSLC_HEADERS} ${QSLC_SOURCES})
target_link_libraries(qslc qslparser Qt5::Core)

install(TARGETS qslc
	    EXPORT QSLCConfig
	    RUNTIME DESTINATION ${BINDIR}
		LIBRARY DESTINATION ${LIBDIR}
		ARCHIVE DESTINATION ${LIBDIR})
install(EXPORT QSLCConfig
	    NAMESPACE QSLC_
	    DESTINATION ${CMAKEDIR}/QSLC)
configure_file(../QSLVersion.cmake.in QSLCConfigVersion.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QSLCConfigVersion.cmake
	    DESTINATION ${CMAKEDIR}/QSLC)
