SET (SOURCES
	${CMAKE_CURRENT_BINARY_DIR}/Module.cpp
	ModuleImpl.cpp
)

SET (HEADERS
	 Module.h
)


# Generate 'Module.cpp' and 'Module.h' from ''Module''
add_custom_command (
  OUTPUT           ${CMAKE_CURRENT_BINARY_DIR}/Module.cpp
  OUTPUT           ${CMAKE_CURRENT_BINARY_DIR}/Module.h
  COMMAND          ${PROJECT_BINARY_DIR}/apps/samsonModuleParser/samsonModuleParser ${CMAKE_CURRENT_SOURCE_DIR}/Module
  COMMAND          /bin/cp ${CMAKE_CURRENT_SOURCE_DIR}/Module.cpp ${CMAKE_CURRENT_BINARY_DIR}/Module.cpp
  MAIN_DEPENDENCY  Module
)


include_directories("${PROJECT_SOURCE_DIR}")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/protocol")

ADD_LIBRARY(sna MODULE ${SOURCES} ${HEADERS})

INSTALL(TARGETS sna DESTINATION ~/.samson/modules/sna)
INSTALL(TARGETS sna DESTINATION /var/samson/modules/sna)

INSTALL(FILES ${HEADERS} DESTINATION samson/modules/sna)
