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
operations/compute_cliques.h
operations/compute_cliques_from_spread_graph.h
operations/spread_nodes_to_strong_connections.h
AUIntStatistics.h
)

#set (CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas -D_LARGEFILE64_SOURCE -D_GNU_SOURCE -g ")

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

ADD_LIBRARY(sna MODULE ${SOURCES} ${HEADERS} ${OPERATION_HEADERS})

INSTALL(TARGETS sna DESTINATION /opt/samson/modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/sna)





