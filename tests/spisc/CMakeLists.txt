add_test(NAME spisc-complete-gen
		 COMMAND spisc -d ${CMAKE_CURRENT_BINARY_DIR} complete.spis
		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME spisc-complete-compile
		 COMMAND ${CMAKE_CXX_COMPILER} -fPIC -std=c++11 -o ${CMAKE_CURRENT_BINARY_DIR}/spisc-complete main.cpp
		         -I${CMAKE_CURRENT_BINARY_DIR} ${TEST_CFLAGS} ${TEST_LDFLAGS}
		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME spisc-complete-qtype-gen
		 COMMAND spisc -d ${CMAKE_CURRENT_BINARY_DIR} -q complete.spis
		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME spisc-complete-qtype-compile
		 COMMAND ${CMAKE_CXX_COMPILER} -fPIC -std=c++11 -o ${CMAKE_CURRENT_BINARY_DIR}/spisc-complete-qtype main.cpp
		         -I${CMAKE_CURRENT_BINARY_DIR} ${TEST_CFLAGS} ${TEST_LDFLAGS}
		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
