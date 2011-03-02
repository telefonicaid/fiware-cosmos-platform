SET (SOURCES
    main_samsonSetup.cpp
)

SET (HEADERS
)

SET (LIBS
    samsonNetwork
    samsonMemory
    pa
    lm
    pthread
    protobuf
    samsonCommon
)

include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")


link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/parseArgs")

# SAMSON Executable
ADD_EXECUTABLE(samsonSetup ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonSetup ${LIBS})

INSTALL(TARGETS               samsonSetup
        RUNTIME DESTINATION   bin)
