set(QSLDUMP_HEADERS)
set(QSLDUMP_SOURCES
	main.cpp)
add_executable(qsldump ${QSLDUMP_HEADERS} ${QSLDUMP_SOURCES})
target_link_libraries(qsldump qsld Qt5::Core)

install(TARGETS qsldump
	    RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib)
