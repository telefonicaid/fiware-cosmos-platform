SET (SOURCES
	SamsonWorker.cpp
)

SET (HEADERS
	SamsonWorker.h
)


include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")


# SAMSON Executable
ADD_EXECUTABLE(samsonWorker ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonWorker samsonNetworkStatic samsonCoreStatic samsonCommonStatic  protobuf pthread dl curses)

INSTALL(TARGETS               samsonWorker
        RUNTIME DESTINATION   bin)
