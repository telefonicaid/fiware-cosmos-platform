SET (SOURCES
	Module.cpp
	ModuleImpl.cpp
)

SET (HEADERS
	 Module.h
)

# Generate 'Module.cpp' and 'Module.h' from ''Module''
add_custom_command (
  OUTPUT           ${CMAKE_CURRENT_SOURCE_DIR}/Module.cpp
  OUTPUT           ${CMAKE_CURRENT_SOURCE_DIR}/Module.h
  COMMAND          ${PROJECT_BINARY_DIR}/apps/samsonModuleParser/samsonModuleParser ${CMAKE_CURRENT_SOURCE_DIR}/Module
  MAIN_DEPENDENCY  Module
)

include_directories("${PROJECT_SOURCE_DIR}/libs/common")

ADD_LIBRARY(example MODULE ${SOURCES} ${HEADERS})

INSTALL(TARGETS example DESTINATION ~/.samson/modules/base)
INSTALL(TARGETS example DESTINATION /var/samson/modules/base)

INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/example)
