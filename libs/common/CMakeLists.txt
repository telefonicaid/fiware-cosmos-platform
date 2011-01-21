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
)

# Generate 'samson.pb.h' and 'samson.pb.cc' from ''samson.proto''
add_custom_command (
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/samson.pb.cc
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/samson.pb.h
  COMMAND protoc ${CMAKE_CURRENT_SOURCE_DIR}/samson.proto --cpp_out=${CMAKE_CURRENT_SOURCE_DIR} --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
  MAIN_DEPENDENCY samson.proto
)


include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")

ADD_LIBRARY(samsonCommonStatic STATIC ${SOURCES} ${HEADERS} ${HEADERS_SAMSON})
TARGET_LINK_LIBRARIES(samsonCommonStatic  protobuf lm z)

SET_TARGET_PROPERTIES(samsonCommonStatic PROPERTIES OUTPUT_NAME samsonCommon)

# ADD_LIBRARY(samsonCommonShared SHARED ${SOURCES} ${HEADERS} ${HEADERS_SAMSON})
# TARGET_LINK_LIBRARIES(samsonCommonShared  protobuf)
# SET_TARGET_PROPERTIES(samsonCommonShared PROPERTIES OUTPUT_NAME samsonCommon)



