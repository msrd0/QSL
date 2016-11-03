add_test(NAME qslc-complete-gen
		 COMMAND qslc -d ${CMAKE_CURRENT_BINARY_DIR} complete.qsl
		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME qslc-complete-compile
		 COMMAND ${CMAKE_CXX_COMPILER} -fPIC -std=c++11 -o ${CMAKE_CURRENT_BINARY_DIR}/qslc-complete main.cpp
		         -I${CMAKE_CURRENT_BINARY_DIR} ${TEST_CFLAGS} ${TEST_LDFLAGS}
		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME qslc-complete-qtype-gen
		 COMMAND qslc -d ${CMAKE_CURRENT_BINARY_DIR} -q complete.qsl
		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME qslc-complete-qtype-compile
		 COMMAND ${CMAKE_CXX_COMPILER} -fPIC -std=c++11 -o ${CMAKE_CURRENT_BINARY_DIR}/qslc-complete-qtype main.cpp
		         -I${CMAKE_CURRENT_BINARY_DIR} ${TEST_CFLAGS} ${TEST_LDFLAGS}
		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
