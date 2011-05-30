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

SET( STATIC_LIBS
     samsonWorkerLib
     samsonControllerLib
     delilahLib
     engine
	samsonModule
	samsonDataLib
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
readline
curses
${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonLocal ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonLocal ${STATIC_LIBS})

INSTALL(TARGETS               samsonLocal
        RUNTIME DESTINATION   bin)


