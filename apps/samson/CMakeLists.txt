SET (SOURCES
	main.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_BINARY_DIR}/libs/protocol")

#link_directories("${PROJECT_BINARY_DIR}/libs/core")
#link_directories("${PROJECT_BINARY_DIR}/libs/protocol")


# SAMSON Executable
ADD_EXECUTABLE(samson ${SOURCES})
TARGET_LINK_LIBRARIES(samson samsonProtocolsStatic samsonCoreStatic protobuf pthread dl curses)

INSTALL(TARGETS               samson
        RUNTIME DESTINATION   bin)
