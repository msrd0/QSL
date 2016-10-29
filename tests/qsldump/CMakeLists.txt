add_executable(createdumpdb main.cpp)
target_compile_definitions(createdumpdb PUBLIC TEST_CREATE)
target_link_libraries(createdumpdb Qt5::Core Qt5::Sql)

# first create the db
add_test(NAME qsldump-createdb
	     COMMAND createdumpdb /tmp/qsldumptest.db)

# then dump it
add_test(NAME qsldump-dump
	     COMMAND qsldump -d sqlite -o /tmp/qsldumptest.qsl /tmp/qsldumptest.db)
set_tests_properties(qsldump-dump PROPERTIES ENVIRONMENT "LD_LIBRARY_PATH=${CMAKE_CURRENT_BINARY_DIR}/../../core/driver/sqlite")

# next run qslc
add_test(NAME qsldump-qslc
	     COMMAND qslc -d ${CMAKE_CURRENT_BINARY_DIR} /tmp/qsldumptest.qsl)

# and compile it
add_test(NAME qsldump-compile
	     COMMAND ${CMAKE_CXX_COMPILER} -fPIC -std=c++11 -o ${CMAKE_CURRENT_BINARY_DIR}/qsldump-compile main.cpp
		         -DTEST_COMPILE -I${CMAKE_CURRENT_BINARY_DIR} ${TEST_CFLAGS} ${TEST_LDFLAGS}
		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
