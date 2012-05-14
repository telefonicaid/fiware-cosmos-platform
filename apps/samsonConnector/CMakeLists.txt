
SET (HEADERS
	BufferList.h
	BufferProcessor.h
	Channel.h
	Connection.h
	ConnectorCommand.h
	DiskItem.h
	FileDescriptorConnection.h
	Item.h
	ListenerItem.h
	SamsonConnector.h
	SamsonItem.h
	ServerConnection.h
	TrafficStatistics.h
	common.h
)

SET (SOURCES
	BufferList.cpp
	BufferProcessor.cpp
	Channel.cpp
	Connection.cpp
	ConnectorCommand.cpp
	DiskItem.cpp
	FileDescriptorConnection.cpp
	Item.cpp
	ListenerItem.cpp
	SamsonConnector.cpp
	SamsonItem.cpp
	ServerConnection.cpp
	TrafficStatistics.cpp
	common.cpp
)

SET( STATIC_LIBS
     samsonClient
     samsonWorkerLib
     delilahLib
     engine
     samsonModule
     samsonStream
     samsonIsolated
     samsonNetwork
     samsonCommon
     au
     lm
     pa
     protobuf
     dl
     pthread
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonConnector main_samsonConnector.cpp ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonConnector ${STATIC_LIBS})

INSTALL(TARGETS               samsonConnector
        RUNTIME DESTINATION   bin)


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonConnectorClient main_samsonConnectorClient.cpp  )
TARGET_LINK_LIBRARIES(samsonConnectorClient ${STATIC_LIBS})

INSTALL(TARGETS               samsonConnectorClient
        RUNTIME DESTINATION   bin)





