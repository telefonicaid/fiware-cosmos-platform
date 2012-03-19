SET (SOURCES
    main_samsonConnector.cpp
	SamsonConnector.cpp
	SamsonConnectorConnection.cpp
	SamsonConnection.cpp
	Block.cpp
	BufferProcessor.cpp
)

SET (HEADERS
	common.h
	SamsonConnector.h
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
	 tables
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

