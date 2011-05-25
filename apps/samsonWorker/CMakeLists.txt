SET (SOURCES
    main_samsonWorker.cpp
)

SET (HEADERS
)


SET (LIBS
    samsonWorkerLib
    samsonNetwork
    samsonModule
    samsonCommon
    samsonStream
    engine
    au
    pa
    lm
    protobuf
    pthread
    dl
    readline
    ${COMMON_SAMSON_LIBS}
)

include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/coding")
include_directories("${PROJECT_SOURCE_DIR}/libs/worker")
include_directories("${PROJECT_SOURCE_DIR}/libs/data")
include_directories("${PROJECT_SOURCE_DIR}/libs/disk")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/process")

# SAMSON Executable
ADD_EXECUTABLE(samsonWorker ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonWorker ${LIBS})

INSTALL(TARGETS               samsonWorker
        RUNTIME DESTINATION   bin)
