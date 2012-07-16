SET (SOURCES
	LogsDataSet.cpp
	SamsonPushLogsConnectionsManager.cpp
    main_samsonPushLogs.cpp
)

SET (HEADERS
	LogsDataSet.h
	SamsonPushLogsConnectionsManager.h
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

ADD_EXECUTABLE(samsonPushLogs ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonPushLogs ${STATIC_LIBS})

INSTALL(TARGETS               samsonPushLogs
        RUNTIME DESTINATION   bin)

