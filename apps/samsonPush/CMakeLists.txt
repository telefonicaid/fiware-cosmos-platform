SET (SOURCES
	SamsonPushConnectionsManager.cpp
    main_samsonPush.cpp
)

SET (HEADERS
	SamsonPushConnectionsManager.h
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

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonPush ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonPush ${STATIC_LIBS})

INSTALL(TARGETS               samsonPush
        RUNTIME DESTINATION   bin)

