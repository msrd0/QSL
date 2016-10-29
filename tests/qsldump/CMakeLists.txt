add_executable(createdumpdb main.cpp)
target_compile_definitions(createdumpdb PUBLIC TEST_CREATE)
target_link_libraries(createdumpdb Qt5::Core Qt5::Sql)
add_test(NAME qsldump-createdb
	     COMMAND createdumpdb /tmp/qsldumptest.db)
add_test(NAME qsldump-dump
	     COMMAND qsldump -d sqlite -o /tmp/qsldumptest.qsl /tmp/qsldumptest.db)
add_test(NAME qsldump-qslc
	     COMMAND qslc -d ${CMAKE_CURRENT_BINARY_DIR} /tmp/qsldumptest.qsl)
add_test(NAME qsldump-compile
	     COMMAND ${CMAKE_CXX_COMPILER} -fPIC -std=c++11 -o ${CMAKE_CURRENT_BINARY_DIR}/qsldump-compile main.cpp
		         -DTEST_COMPILE -I${CMAKE_CURRENT_BINARY_DIR} ${TEST_CFLAGS} ${TEST_LDFLAGS}
		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
