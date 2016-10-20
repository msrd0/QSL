add_test(NAME qslc-complete-gen
		 COMMAND qslc -d ${CMAKE_CURRENT_BINARY_DIR} complete.qsl
		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
add_test(NAME qslc-complete-compile
		 COMMAND ${CMAKE_CXX_COMPILER} -fPIC -std=c++11 -o ${CMAKE_CURRENT_BINARY_DIR}/qslc-complete main.cpp
		         -I${CMAKE_CURRENT_BINARY_DIR}
		         -isystem ${CMAKE_CURRENT_BINARY_DIR}/../../
		         -isystem ${CMAKE_CURRENT_SOURCE_DIR}/../../core/
		         -isystem /usr/include/qt
		         -isystem /usr/include/qt/QtCore
		         -isystem /usr/include/qt/QtSql
		         -L${CMAKE_CURRENT_BINARY_DIR}/../../core/
		         -L${CMAKE_CURRENT_BINARY_DIR}/../../core/drivers/
		         -lqsld -lqslcore -lQt5Core -lQt5Sql
		 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
