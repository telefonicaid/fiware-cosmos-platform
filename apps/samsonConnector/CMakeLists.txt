
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
	InterChannelItem.h
	SamsonConnector.h
	SamsonItem.h
	ServerConnection.h
	TrafficStatistics.h
	common.h
	message.pb.h
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
	InterChannelItem.cpp
	SamsonConnector.cpp
	SamsonItem.cpp
	ServerConnection.cpp
	TrafficStatistics.cpp
	common.cpp
	message.pb.cc
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





