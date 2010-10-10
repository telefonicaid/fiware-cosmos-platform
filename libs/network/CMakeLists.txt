SET (HEADERS
	samson.pb.h
	network.h
	packet.h
	endpoint.h
)

SET (SOURCES
	 samson.pb.cc
	 network.cpp
	packet.cpp
	endpoint.cpp
)

# Generate 'samson.pb.h' and 'samson.pb.cc' from ''samson.proto''
add_custom_command (
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/samson.pb.cc
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/samson.pb.h
  COMMAND /usr/local/bin/protoc ${CMAKE_CURRENT_SOURCE_DIR}/samson.proto --cpp_out=${CMAKE_CURRENT_BINARY_DIR} --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
  MAIN_DEPENDENCY samson.proto
)

include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")


ADD_LIBRARY(samsonNetworkStatic STATIC ${SOURCES} ${HEADERS} )
ADD_LIBRARY(samsonNetworkShared SHARED ${SOURCES} ${HEADERS} )

TARGET_LINK_LIBRARIES( samsonNetworkStatic  protobuf )
TARGET_LINK_LIBRARIES( samsonNetworkShared  protobuf )

SET_TARGET_PROPERTIES(samsonNetworkShared PROPERTIES OUTPUT_NAME samsonNetwork)
SET_TARGET_PROPERTIES(samsonNetworkStatic PROPERTIES OUTPUT_NAME samsonNetwork)
