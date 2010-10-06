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

ADD_LIBRARY(sna MODULE ${SOURCES} ${HEADERS})

INSTALL(TARGETS sna DESTINATION ~/.samson/modules/base)
INSTALL(TARGETS sna DESTINATION /var/samson/modules/base)

INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/sna)
