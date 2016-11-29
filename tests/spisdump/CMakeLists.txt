add_executable(createdumpdb main.cpp)
target_compile_definitions(createdumpdb PUBLIC TEST_CREATE)
target_link_libraries(createdumpdb Qt5::Core Qt5::Sql)

# first create the db
add_test(NAME spisdump-createdb
	     COMMAND createdumpdb /tmp/spisdumptest.db)

# then dump it
add_test(NAME spisdump-dump
	     COMMAND spisdump -d sqlite -o /tmp/spisdumptest.spis /tmp/spisdumptest.db)
set_tests_properties(spisdump-dump PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}/../../core/driver/sqlite")

# next run spisc
add_test(NAME spisdump-spisc
	     COMMAND spisc -d ${CMAKE_CURRENT_BINARY_DIR} /tmp/spisdumptest.spis)

# and compile it
add_test(NAME spisdump-compile
	     COMMAND ${CMAKE_CXX_COMPILER} -fPIC -std=c++11 -o ${CMAKE_CURRENT_BINARY_DIR}/spisdump-compile main.cpp
		         -DTEST_COMPILE -I${CMAKE_CURRENT_BINARY_DIR} ${TEST_CFLAGS} ${TEST_LDFLAGS}
		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
