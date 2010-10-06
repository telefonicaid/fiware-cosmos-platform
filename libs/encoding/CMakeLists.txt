SET( SOURCES
)

SET( HEADERS
Common.h
Buffers.h
DynamicBuffer.h
KVDeSerializer.h
KVSerializer.h
StaticBuffer.h
Test.h
VarInt.h
)

include_directories("${PROJECT_BINARY_DIR}/libs/protocol")

# Executable
ADD_LIBRARY( samsonEncoding encoding.cpp ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( samsonEncoding samsonProtocol protobuf pthread dl curses)



