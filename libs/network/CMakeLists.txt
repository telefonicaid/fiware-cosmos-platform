SET (HEADERS
	Message.h
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
	easyzlib.h
	HostMgr.h
	Host.h
)

SET (SOURCES
	Message.cpp
	Network.cpp
	NetworkInterface.cpp
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
	easyzlib.c
	HostMgr.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")

ADD_LIBRARY(samsonNetworkStatic STATIC ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( samsonNetworkStatic  protobuf logMsgStatic samsonMemoryStatic )
SET_TARGET_PROPERTIES(samsonNetworkStatic PROPERTIES OUTPUT_NAME samsonNetwork)

# ADD_LIBRARY(samsonNetworkShared SHARED ${SOURCES} ${HEADERS} )
# TARGET_LINK_LIBRARIES( samsonNetworkShared  protobuf logMsgShared )
# SET_TARGET_PROPERTIES(samsonNetworkShared PROPERTIES OUTPUT_NAME samsonNetwork)
