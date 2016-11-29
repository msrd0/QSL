set(SPISDUMP_HEADERS)
set(SPISDUMP_SOURCES
	main.cpp)
add_executable(spisdump ${SPISDUMP_HEADERS} ${SPISDUMP_SOURCES})
target_link_libraries(spisdump spiscore spisd Qt5::Core)

install(TARGETS spisdump
	    RUNTIME DESTINATION ${BINDIR}
		LIBRARY DESTINATION ${LIBDIR})
