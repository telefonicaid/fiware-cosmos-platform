cmake_minimum_required(VERSION 2.6)

SET (SOURCES
    Module.h
    Module.cpp
    common.cpp
    CliquesSolver.cpp
    AUIntStatistics.cpp
)

SET (OPERATION_HEADERS
    common.h
    CliquesSolver.h
AUIntStatistics.h
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

include_directories("${PROJECT_SOURCE_DIR}")

ADD_LIBRARY(sna_light MODULE ${SOURCES} ${HEADERS} ${OPERATION_HEADERS})

INSTALL(TARGETS sna_light DESTINATION /opt/samson/modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/sna_light)





