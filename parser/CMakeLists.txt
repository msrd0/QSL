include_directories(include)
add_library(qslparser SHARED
        include/database.h include/parser.h include/table.h
        src/database.cpp src/parser.cpp src/table.cpp
    )
target_link_libraries(qslparser Qt5::Core Qt5::Sql)
set_target_properties(qslparser PROPERTIES VERSION ${QSL_VERSION} SOVERSION ${QSL_MAJOR})

install(TARGETS qslparser
	    RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib)
