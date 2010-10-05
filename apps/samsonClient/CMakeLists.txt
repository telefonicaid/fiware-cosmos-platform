SET (SOURCES
	main_client.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/protocol")
link_directories("${PROJECT_BINARY_DIR}/libs/core")
link_directories("${PROJECT_BINARY_DIR}/libs/protocol")

# SAMSON Client
ADD_EXECUTABLE(samsonClient ${SOURCES})
TARGET_LINK_LIBRARIES(samsonClient samsonProtocols samsonCore protobuf pthread dl curses)

INSTALL(TARGETS               samsonClient
        RUNTIME DESTINATION   bin)
