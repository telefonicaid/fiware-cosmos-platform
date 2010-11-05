SET (SOURCES
    main_samsonWorker.cpp
)

SET (HEADERS
)


include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/data")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/process")

link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")

# SAMSON Executable
ADD_EXECUTABLE(samsonWorker ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonWorker samsonNetworkStatic samsonCoreStatic samsonCommonStatic lm protobuf pthread dl curses)

INSTALL(TARGETS               samsonWorker
        RUNTIME DESTINATION   bin)
