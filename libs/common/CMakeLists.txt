SET (HEADERS
	samson.pb.h
	ports.h
	samsonDirectories.h
	samsonLogMsg.h
	SamsonSetup.h
	traces.h
	MessagesOperations.h
	daemonize.h
	EnvironmentOperations.h
	platformProcesses.h
	Process.h
	samsonConfig.h
	coding.h
	MemoryTags.h
)

SET (SOURCES
	samson.pb.cc
	SamsonSetup.cpp
	traces.cpp
	MessagesOperations.cpp
	daemonize.cpp
	EnvironmentOperations.cpp
	platformProcesses.cpp
	coding.cpp
)

# Generate 'samson.pb.h' and 'samson.pb.cc' from ''samson.proto''
add_custom_command (
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/samson.pb.cc
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/samson.pb.h
  COMMAND protoc ${CMAKE_CURRENT_SOURCE_DIR}/samson.proto --cpp_out=${CMAKE_CURRENT_SOURCE_DIR} --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
  MAIN_DEPENDENCY samson.proto
)

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/engine")



# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonCommon STATIC ${SOURCES} ${HEADERS})
