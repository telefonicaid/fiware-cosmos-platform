
SET (HEADERS
	common.h
	message.pb.h
	LogManager.h
	BufferList.h
	BufferProcessor.h
	Channel.h
	Connection.h
	ConnectorCommand.h
	FileDescriptorConnection.h
	Adaptor.h
	DiskAdaptor.h
	ListenerAdaptor.h
	SamsonAdaptor.h
	ChannelAdaptor.h
	StreamConnector.h
	StreamConnectorService.h
	ServerConnection.h
	TrafficStatistics.h
)

SET (SOURCES
	common.cpp
	message.pb.cc
	LogManager.cpp
	BufferList.cpp
	BufferProcessor.cpp
	Channel.cpp
	Connection.cpp
	ConnectorCommand.cpp
	FileDescriptorConnection.cpp
	Adaptor.cpp
	DiskAdaptor.cpp
	ListenerAdaptor.cpp
	SamsonAdaptor.cpp
	ChannelAdaptor.cpp
	StreamConnector.cpp
	StreamConnectorService.cpp
	ServerConnection.cpp
	TrafficStatistics.cpp
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
	au
     pa
     protobuf
     dl
     pthread
)

# Generate 'message.pb.h' and 'message.pb.cc' from ''message.proto''
add_custom_command (
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/message.pb.cc
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/message.pb.h
  COMMAND protoc ${CMAKE_CURRENT_SOURCE_DIR}/message.proto --cpp_out=${CMAKE_CURRENT_SOURCE_DIR} --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
  MAIN_DEPENDENCY message.proto
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(streamConnector main_streamConnector.cpp ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(streamConnector ${STATIC_LIBS})

INSTALL(TARGETS               streamConnector
        RUNTIME DESTINATION   bin)


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(streamConnectorClient main_streamConnectorClient.cpp )
TARGET_LINK_LIBRARIES(streamConnectorClient ${STATIC_LIBS})

INSTALL(TARGETS               streamConnectorClient
        RUNTIME DESTINATION   bin)





