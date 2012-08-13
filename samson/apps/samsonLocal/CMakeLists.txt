SET (SOURCES
    FakeEndpoint.cpp
    NetworkFake.cpp
    NetworkFakeCenterPacket.cpp
    NetworkCenter.cpp
    main_samsonLocal.cpp
)

SET (HEADERS
    FakeEndpoint.h
    NetworkFake.h
    NetworkCenter.h
    NetworkFakeCenterPacket.h
)

# SAMSON-956 - iconv functions are built into glibc on Linux but require a 
# separate library on OS X
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set (ICONV_LIB "iconv")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
    set (SOLARIS_LIBS "-lsocket -lnsl -ljson-c")
endif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")

SET( STATIC_LIBS
    samsonWorkerLib
    delilahLib
    engine
    samsonModule
    samsonStream
    samsonIsolated
    samsonNetwork
    samsonCommon
	samson_system
    au
    lm
	au
    ${ICONV_LIB}
    ${SOLARIS_LIBS}
    pa
    protobuf
    dl
    pthread
    ${COMMON_SAMSON_LIBS}
)


# Include directories
# ------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")
#INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})        # Required for QT intermediate files to compile

# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonLocal ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonLocal ${STATIC_LIBS})
#TARGET_LINK_LIBRARIES( samsonLocal ${QT_LIBRARIES} )

INSTALL(TARGETS               samsonLocal
        RUNTIME DESTINATION   bin)


