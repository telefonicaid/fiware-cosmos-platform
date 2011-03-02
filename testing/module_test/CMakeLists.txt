
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

# This is a spetial module since it is compiled with the platform only for testing
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/testing/module_test")

ADD_LIBRARY(module_test MODULE ${SOURCES} ${HEADERS})

add_custom_command ( TARGET module_test POST_BUILD   COMMAND mkdir ${PROJECT_BINARY_DIR}/testing/modules 2>/dev/null  || true )
add_custom_command ( TARGET module_test POST_BUILD   COMMAND cp -f *.so ${PROJECT_BINARY_DIR}/testing/modules || true  )

