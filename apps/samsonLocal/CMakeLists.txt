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
samsonCore
samsonDataLib
samsonNetwork
samsonCommon
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
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/data")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/delilah")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/coding
include_directories("${PROJECT_SOURCE_DIR}/libs/disk")")
include_directories("${PROJECT_SOURCE_DIR}/libs/process")
include_directories("${PROJECT_SOURCE_DIR}/libs/controller")
include_directories("${PROJECT_SOURCE_DIR}/libs/worker")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonLocal ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonLocal ${STATIC_LIBS})

INSTALL(TARGETS               samsonLocal
        RUNTIME DESTINATION   bin)


