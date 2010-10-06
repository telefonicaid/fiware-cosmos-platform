SET (HEADER
	DataContainer.h
	DataCreator.h
	DataType.h
	samson_module.h
)

SET (SOURCES
	DataContainer.cpp
	DataCreator.cpp
	DataType.cpp
	samson_module.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/protocol")
link_directories("${PROJECT_BINARY_DIR}/libs/core")
link_directories("${PROJECT_BINARY_DIR}/libs/protocol")


# Executable
ADD_EXECUTABLE(samsonModuleParser ${SOURCES})
TARGET_LINK_LIBRARIES(samsonModuleParser samsonCore samsonProtocols protobuf pthread dl curses)

INSTALL(TARGETS               samsonModuleParser
        RUNTIME DESTINATION   bin)
