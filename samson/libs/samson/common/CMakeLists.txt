SET (HEADERS
	common.h
	Logs.h
	KVInfo.h
	FullKVInfo.h
	BlockKVInfo.h
	KVRange.h
	KVRangeDivision.h
	KVHeader.h
	KVFile.h
	Rate.h	
    NotificationMessages.h
	samson.pb.h
	gpb_operations.h
	status.h
	Visualitzation.h
	ports.h
	samsonDirectories.h
	SamsonSetup.h
	MessagesOperations.h
	EnvironmentOperations.h
	coding.h
	KVInputVector.h
	samsonVars.h
    MemoryCheck.h
    SamsonDataSet.h
    SamsonFile.h
)

SET (SOURCES
	common.cpp
	Logs.cpp
	KVInfo.cpp
	FullKVInfo.cpp
	BlockKVInfo.cpp
	KVRange.cpp
    KVRangeDivision.cpp 
	KVHeader.cpp
	KVFile.cpp
    Rate.cpp
	samson.pb.cc
	gpb_operations.cpp
	SamsonSetup.cpp
	MessagesOperations.cpp
	EnvironmentOperations.cpp
	coding.cpp
	KVInputVector.cpp
	status.cpp
	Visualitzation.cpp 
    MemoryCheck.cpp
    SamsonDataSet.cpp
    SamsonFile.cpp
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
TARGET_LINK_LIBRARIES(samsonCommon engine)
ADD_DEPENDENCIES(samsonCommon samson.pb.h)
ADD_DEPENDENCIES(samsonCommon samson.pb.cc)

INSTALL(FILES Logs.h DESTINATION include/samson/common)
