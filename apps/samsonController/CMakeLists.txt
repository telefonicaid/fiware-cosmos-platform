SET (SOURCES
    main_samsonController.cpp
)

SET (HEADERS

)


include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/data")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")

link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")


# SAMSON Executable
ADD_EXECUTABLE(samsonController ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonController samsonNetworkStatic samsonCoreStatic samsonCommonStatic lm protobuf pthread dl curses)

INSTALL(TARGETS               samsonController
        RUNTIME DESTINATION   bin)
