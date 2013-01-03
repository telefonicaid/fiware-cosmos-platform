SET (SOURCES
  SamsonPushConnectionsManager.cpp
    main_samsonPush.cpp
)

SET (HEADERS
  SamsonPushConnectionsManager.h
)

if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
    set (SOLARIS_LIBS "-lsocket -lnsl")
endif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
SET( STATIC_LIBS
    samsonClient
    samsonWorkerLib
    delilahLib
    engine
    samsonModule
    samsonStream
    samsonNetwork
    samsonCommon
    zookeeper_mt
    au
    lm
    au
    pa
    protobuf
    dl
    pthread
    ${SOLARIS_LIBS}
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

