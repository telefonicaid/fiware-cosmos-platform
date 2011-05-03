SET (SOURCES
    main_samsonSpawner.cpp
	SamsonSpawner.cpp
	processList.cpp
)

SET (HEADERS
	SamsonSpawner.h
	processList.h
)

SET (LIBS
    samsonNetwork
    engine
    pa
    lm
    au
    pthread
    protobuf
    samsonCommon
    ${COMMON_SAMSON_LIBS}
    au
)

include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/engine")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")


link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/parseArgs")

# SAMSON Executable
ADD_EXECUTABLE(samsonSpawner2 ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonSpawner2 ${LIBS})

INSTALL(TARGETS               samsonSpawner2
        RUNTIME DESTINATION   bin)
