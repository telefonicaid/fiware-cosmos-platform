SET (HEADERS	
	ClusterNodeType.h

	PacketQueue.h
	NetworkManager.h
	NetworkConnection.h

	NetworkInterface.h
	CommonNetwork.h
	WorkerNetwork.h
	DelilahNetwork.h

	NodeIdentifier.h

	PacketReceivedNotification.h
	Packet.h
	Message.h
)

SET (SOURCES
	ClusterNodeType.cpp

	PacketQueue.cpp
	NetworkManager.cpp
	NetworkConnection.cpp

	NetworkInterface.cpp
	CommonNetwork.cpp
	WorkerNetwork.cpp
	DelilahNetwork.cpp

	NodeIdentifier.cpp

	PacketReceivedNotification.cpp
	Packet.cpp
	Message.cpp
)

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonNetwork STATIC ${SOURCES} ${HEADERS} )

