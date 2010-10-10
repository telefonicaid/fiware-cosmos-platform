SET (SOURCES
	delilah.cpp
)


include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")


# SAMSON Executable
ADD_EXECUTABLE(delilah ${SOURCES})
TARGET_LINK_LIBRARIES(delilah samsonNetworkStatic samsonCoreStatic samsonCommonStatic   protobuf pthread dl curses)

INSTALL(TARGETS               delilah
        RUNTIME DESTINATION   bin)
