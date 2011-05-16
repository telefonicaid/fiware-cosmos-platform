cmake_minimum_required(VERSION 2.6)

SET (SOURCES
    Module.h
    Module.cpp
)

SET (OPERATION_HEADERS
)

# Install headers to be used by other modules

SET (HEADERS
)

#Generate 'Module.cpp' and 'Module.h' from ''Module''
add_custom_command (
  OUTPUT           ${CMAKE_CURRENT_SOURCE_DIR}/Module.h
  OUTPUT	   ${CMAKE_CURRENT_SOURCE_DIR}/Module.cpp
  COMMAND          samsonModuleParser ${CMAKE_CURRENT_SOURCE_DIR}/module
  MAIN_DEPENDENCY  module
)

include_directories("${PROJECT_SOURCE_DIR}",  "../../trunk/libs/logMsg")

ADD_LIBRARY(<ModuleName> MODULE ${SOURCES} ${HEADERS} ${OPERATION_HEADERS})

INSTALL(TARGETS <ModuleName> DESTINATION /opt/samson/modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/<ModuleName>)

ENABLE_TESTING()
ADD_SUBDIRECTORY(testing)     # Test module and ctest stuff

