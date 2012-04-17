SET (HEADERS
	common.h
	KVInfo.h
	FullKVInfo.h
	KVRange.h
	KVHeader.h
	KVFile.h
	Rate.h	
    NotificationMessages.h
	samson.pb.h
	status.h
	Visualitzation.h
	ports.h
	samsonDirectories.h
	samsonLogMsg.h
	SamsonSetup.h
	traces.h
	MessagesOperations.h
	EnvironmentOperations.h
	samsonConfig.h
	coding.h
	MemoryTags.h
	KVInputVector.h
	samsonVars.h
    MemoryCheck.h
)

SET (SOURCES
	KVInfo.cpp
	FullKVInfo.cpp
	KVRange.cpp
	KVHeader.cpp
	KVFile.cpp
    Rate.cpp
	samson.pb.cc
	SamsonSetup.cpp
	traces.cpp
	MessagesOperations.cpp
	EnvironmentOperations.cpp
	coding.cpp
	KVInputVector.cpp
	status.cpp
	Visualitzation.cpp 
    MemoryCheck.cpp
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


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonCommon STATIC ${SOURCES} ${HEADERS})
