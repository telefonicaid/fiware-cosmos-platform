SET (HEADERS
	Message.h
	samson.pb.h
	Network.h
	NetworkInterface.h
	Packet.h
	Endpoint.h
	iomInit.h
	iomServerOpen.h
	iomConnect.h
	iomAccept.h
	iomMsgSend.h
	iomMsgRead.h
	iomMsgAwait.h
	workerStatus.h
	Alarm.h
	Misc.h
	EndpointMgr.h
)

SET (SOURCES
	Message.cpp
	samson.pb.cc
	Network.cpp
	Packet.cpp
	Endpoint.cpp
	iomInit.cpp
	iomServerOpen.cpp
	iomConnect.cpp
	iomAccept.cpp
	iomMsgSend.cpp
	iomMsgRead.cpp
	iomMsgAwait.cpp
	workerStatus.cpp
	Alarm.cpp
	Misc.cpp
	EndpointMgr.cpp
)

# Generate 'samson.pb.h' and 'samson.pb.cc' from ''samson.proto''
add_custom_command (
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/samson.pb.cc
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/samson.pb.h
  COMMAND protoc ${CMAKE_CURRENT_SOURCE_DIR}/samson.proto --cpp_out=${CMAKE_CURRENT_SOURCE_DIR} --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
  MAIN_DEPENDENCY samson.proto
)

include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")

ADD_LIBRARY(samsonNetworkStatic STATIC ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( samsonNetworkStatic  protobuf logMsgStatic )
SET_TARGET_PROPERTIES(samsonNetworkStatic PROPERTIES OUTPUT_NAME samsonNetwork)

# ADD_LIBRARY(samsonNetworkShared SHARED ${SOURCES} ${HEADERS} )
# TARGET_LINK_LIBRARIES( samsonNetworkShared  protobuf logMsgShared )
# SET_TARGET_PROPERTIES(samsonNetworkShared PROPERTIES OUTPUT_NAME samsonNetwork)
