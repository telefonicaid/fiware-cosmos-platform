SET (SOURCES
	common.cpp
    main_samsonConnector.cpp
	SamsonConnector.cpp
	SamsonConnectorListener.cpp
	DiskConnection.cpp
	ServerConnection.cpp
	SamsonConnectorConnection.cpp
	SamsonConnection.cpp
	Block.cpp
	BufferProcessor.cpp
)

SET (HEADERS
	common.h
	SamsonConnector.h
	SamsonConnectorListener.h
	DiskConnection.h
	ServerConnection.h
	SamsonConnectorConnection.h
	SamsonConnection.h
	Block.h
	BufferProcessor.h
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

ADD_EXECUTABLE(samsonConnector ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonConnector ${STATIC_LIBS})

INSTALL(TARGETS               samsonConnector
        RUNTIME DESTINATION   bin)

