cmake_minimum_required(VERSION 2.6)
SET (SOURCES
    Module.h
    Module.cpp
)

# Install headers to be used by other modules

SET (HEADERS
)

SET (OPERATION_HEADERS
operations/parser_words.h
operations/export_words.h
operations/word_count.h
)

#Generate 'Module.cpp' and 'Module.h' from ''Module''
add_custom_command (
  OUTPUT           ${CMAKE_CURRENT_SOURCE_DIR}/Module.h
  OUTPUT	   ${CMAKE_CURRENT_SOURCE_DIR}/Module.cpp
  COMMAND          samsonModuleParser ${CMAKE_CURRENT_SOURCE_DIR}/module
  MAIN_DEPENDENCY  module
)

include_directories("${PROJECT_SOURCE_DIR}")

ADD_LIBRARY(txt MODULE ${SOURCES} ${HEADERS} ${OPERATION_HEADERS})

INSTALL(TARGETS txt DESTINATION /opt/samson/modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/txt)

ENABLE_TESTING()
ADD_SUBDIRECTORY(testing)     # Test module and ctest stuff
