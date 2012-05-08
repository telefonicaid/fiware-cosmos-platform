SET (SOURCES
   	common.cpp
	ConnectorCommand.cpp
	Block.cpp
	BufferProcessor.cpp
	TrafficStatistics.cpp
	SamsonConnector.cpp
	Channel.cpp
	Item.cpp
	Connection.cpp
	SamsonConnectorListener.cpp
	DiskConnection.cpp
	ServerConnection.cpp
	SamsonConnectorConnection.cpp
	SamsonConnection.cpp
)

SET (HEADERS
	common.h
	ConnectorCommand.h
	Block.h
	BufferProcessor.h
	TrafficStatistics.h
	SamsonConnector.h
	Channel.h
	Item.h
	Connection.h
	SamsonConnectorListener.h
	DiskConnection.h
	ServerConnection.h
	SamsonConnectorConnection.h
	SamsonConnection.h
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




