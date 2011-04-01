SET (SOURCES
    main_samsonKiller.cpp
)

SET (HEADERS
)

SET (LIBS
samsonNetwork
samsonCore
samsonCommon
pa
lm
pthread	
protobuf
${COMMON_SAMSON_LIBS}
)

include_directories("${PROJECT_BINARY_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")



link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/parseArgs")

# SAMSON Executable
ADD_EXECUTABLE(samsonKiller ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonKiller ${LIBS})

INSTALL(TARGETS               samsonKiller
        RUNTIME DESTINATION   bin)
