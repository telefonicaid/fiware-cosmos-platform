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
	PacketReceivedNotification.h

	Network2.h
	JobQueue.h
	EndpointManager.h
	Endpoint2.h
	ListenerEndpoint.h
	UnhelloedEndpoint.cpp
	WebListenerEndpoint.h
	WebWorkerEndpoint.h
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
	PacketReceivedNotification.cpp

	Network2.cpp
	JobQueue.cpp
	EndpointManager.cpp
	Endpoint2.cpp
	ListenerEndpoint.cpp
	UnhelloedEndpoint.cpp
	WebListenerEndpoint.cpp
	WebWorkerEndpoint.cpp
)

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/engine")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonNetwork STATIC ${SOURCES} ${HEADERS} )

