cmake_minimum_required(VERSION 2.6)

SET (SOURCES
    Module.h
    Module.cpp
)

SET (OPERATION_HEADERS
operations/add_extern_mark_to_graph.h
operations/compute_graph.h
operations/fake_graph.h
operations/filter_graph.h
operations/filter_graph_map.h
operations/filter_graph_reduce.h
operations/get_graph_from_links.h
operations/parse_graph.h
operations/parse_links.h
operations/select_graph.h
)

# Install headers to be used by other modules

SET (HEADERS
samson/modules/graph/Link.h
samson/modules/graph/Link_base.h
samson/modules/graph/Node.h
samson/modules/graph/Node_base.h
samson/modules/graph/VectorNode.h
samson/modules/graph/VectorNode_base.h
)

#Generate 'Module.cpp' and 'Module.h' from ''Module''
add_custom_command (
  OUTPUT           ${CMAKE_CURRENT_SOURCE_DIR}/Module.h
  OUTPUT	   ${CMAKE_CURRENT_SOURCE_DIR}/Module.cpp
  COMMAND          samsonModuleParser ${CMAKE_CURRENT_SOURCE_DIR}/module
  MAIN_DEPENDENCY  module
)

include_directories("${PROJECT_SOURCE_DIR}")

ADD_LIBRARY(graph MODULE ${SOURCES} ${HEADERS} ${OPERATION_HEADERS})

INSTALL(TARGETS graph DESTINATION /opt/samson/modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/graph)







