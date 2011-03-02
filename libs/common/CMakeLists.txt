SET (HEADERS
	samson.pb.h
	ports.h
	samsonDirectories.h
	samsonLogMsg.h
	Format.h
	LockDebugger.h
	Token.h
	Stopper.h
	Lock.h
	StopLock.h
	SamsonSetup.h
	traces.h
	au_map.h
	Status.h
	MessagesOperations.h
	Console.h
	CommandLine.h
	daemonize.h
	EnvironmentOperations.h
	platformProcesses.h
	Process.h
	Worker.h
)

SET (SOURCES
	samson.pb.cc
	LockDebugger.cpp
	Lock.cpp
	Token.cpp
	Stopper.cpp
	SamsonSetup.cpp
	traces.cpp
	Status.cpp
	MessagesOperations.cpp
	Console.cpp
	Format.cpp
	CommandLine.cpp
	daemonize.cpp
	EnvironmentOperations.cpp
	platformProcesses.cpp
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
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonCommon STATIC ${SOURCES} ${HEADERS})
