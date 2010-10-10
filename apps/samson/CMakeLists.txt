SET (SOURCES
	main.cpp
)


include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")


# SAMSON Executable
ADD_EXECUTABLE(samson ${SOURCES})
TARGET_LINK_LIBRARIES(samson samsonNetworkStatic samsonCoreStatic samsonCommonStatic  protobuf pthread dl curses)

INSTALL(TARGETS               samson
        RUNTIME DESTINATION   bin)
