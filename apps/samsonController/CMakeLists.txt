SET (SOURCES
    main_samsonController.cpp
)

SET (HEADERS

)

SET (LIBS
	samsonControllerLib
	samsonProcess
	samsonNetwork
	samsonCore
	samsonMemory
	samsonDisk
	pa
	lm
	protobuf
	pthread
	dl
	readline
	samsonDataLib
	samsonCommon
	${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/controller")
include_directories("${PROJECT_SOURCE_DIR}/libs/disk")
include_directories("${PROJECT_SOURCE_DIR}/libs/data")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/coding")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonController ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonController ${LIBS})


INSTALL(TARGETS               samsonController
        RUNTIME DESTINATION   bin)
