SET (SOURCES
    SamsonController.cpp
)

SET (HEADERS
    SamsonController.h
)


include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")


# SAMSON Executable
ADD_EXECUTABLE(samsonController ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonController samsonNetworkStatic samsonCoreStatic samsonCommonStatic  protobuf pthread dl curses)

INSTALL(TARGETS               samsonController
        RUNTIME DESTINATION   bin)
