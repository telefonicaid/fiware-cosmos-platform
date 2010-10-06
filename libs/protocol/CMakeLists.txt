SET (HEADERS
	samson.pb.h
)

SET (SOURCES
	 samson.pb.cc
)

# Generate 'samson.pb.h' and 'samson.pb.cc' from ''samson.proto''
add_custom_command (
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/samson.pb.cc
  OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/samson.pb.h
  COMMAND /usr/local/bin/protoc ${CMAKE_CURRENT_SOURCE_DIR}/samson.proto --cpp_out=${CMAKE_CURRENT_BINARY_DIR} --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
  MAIN_DEPENDENCY samson.proto
)


ADD_LIBRARY(samsonProtocolsStatic STATIC ${SOURCES})
#ADD_LIBRARY(samsonProtocolsShared SHARED ${SOURCES})

TARGET_LINK_LIBRARIES( samsonProtocolsStatic  protobuf)
#TARGET_LINK_LIBRARIES( samsonProtocolsShared  protobuf)

#SET_TARGET_PROPERTIES(samsonProtocolsStatic samsonProtocolsShared PROPERTIES OUTPUT_NAME samsonProtocols)
SET_TARGET_PROPERTIES(samsonProtocolsStatic PROPERTIES OUTPUT_NAME samsonProtocols)



