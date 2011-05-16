cmake_minimum_required(VERSION 2.6)


SET (SOURCES
    Module.h
    Module.cpp
)

SET (OPERATION_HEADERS
operations/generator.h
operations/generator2.h
operations/map.h
operations/select.h
operations/reduce.h
operations/reduce2.h
operations/parser.h
operations/s2.h
operations/script.h
operations/error.h
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

ADD_LIBRARY(example MODULE ${SOURCES} ${HEADERS} ${OPERATION_HEADERS})
TARGET_LINK_LIBRARIES( example )

INSTALL(TARGETS example DESTINATION /opt/samson/modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/example)


