include_directories(../core/include)
include_directories(../parser/include)
include_directories(include)
add_executable(qslc
	    include/generator.h
        src/generator.cpp src/main.cpp
    )
target_link_libraries(qslc qslparser Qt5::Core)

install(TARGETS qslc
	    EXPORT QSLCConfig
	    RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib)
install(EXPORT QSLCConfig
	    NAMESPACE QSLC_
	    DESTINATION lib/cmake/QSLC)
configure_file(../QSLVersion.cmake.in QSLCConfigVersion.cmake @ONLY)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/QSLCConfigVersion.cmake
	    DESTINATION lib/cmake/QSLC)
