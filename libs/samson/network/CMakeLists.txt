SET (HEADERS	
	HostMgr.h
	Host.h

	PacketQueue.h
	NetworkManager.h
	NetworkConnection.h
	SocketConnection.h
	FileDescriptor.h
	NetworkListener.h

	NetworkInterface.h
	CommonNetwork.h
	WorkerNetwork.h
	DelilahNetwork.h
	misc.h

	PacketReceivedNotification.h
	Packet.h
	Message.h

	ClusterInformation.h

)

SET (SOURCES
	HostMgr.cpp
	Host.cpp

	PacketQueue.cpp
	NetworkManager.cpp
	NetworkConnection.cpp
	SocketConnection.cpp
	FileDescriptor.cpp 
	NetworkListener.cpp

	NetworkInterface.cpp
	CommonNetwork.cpp
	WorkerNetwork.cpp
	DelilahNetwork.cpp
	misc.cpp

	PacketReceivedNotification.cpp
	Packet.cpp
	Message.cpp

	ClusterInformation.cpp

)

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonNetwork STATIC ${SOURCES} ${HEADERS} )

