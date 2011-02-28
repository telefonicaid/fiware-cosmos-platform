SET (SOURCES
    main_samsonSpawner.cpp
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
ADD_EXECUTABLE(samsonSpawner ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonSpawner ${LIBS})

INSTALL(TARGETS               samsonSpawner
        RUNTIME DESTINATION   bin)
