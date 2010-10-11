SET (SOURCES
	Delilah.cpp
)

SET (HEADERS
    Delilah.h
)


include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")

# SAMSON Executable
ADD_EXECUTABLE(delilah ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(delilah samsonNetworkStatic samsonCoreStatic samsonCommonStatic lm protobuf pthread dl curses)

INSTALL(TARGETS               delilah
        RUNTIME DESTINATION   bin)
