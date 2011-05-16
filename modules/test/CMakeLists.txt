
PROJECT(SamsonTest)
cmake_minimum_required(VERSION 2.6)

SET (SOURCES
    Module.h
    Module.cpp
)

SET (HEADERS
)


#Generate 'Module.cpp' and 'Module.h' from ''Module''
add_custom_command (
  OUTPUT           ${CMAKE_CURRENT_SOURCE_DIR}/Module.h
  OUTPUT	   ${CMAKE_CURRENT_SOURCE_DIR}/Module.cpp
  COMMAND          samsonModuleParser ${CMAKE_CURRENT_SOURCE_DIR}/module
  MAIN_DEPENDENCY  module
)


include_directories("${PROJECT_SOURCE_DIR}")
ADD_LIBRARY(module_test MODULE ${SOURCES} ${HEADERS})

ENABLE_TESTING()
ADD_TEST(test_1 ${PROJECT_SOURCE_DIR}/test_1 )
