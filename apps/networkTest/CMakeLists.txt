SET (SOURCES
    networkTest.cpp
)

SET (HEADERS
)


include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")

link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/parseArgs")
link_directories("${PROJECT_BINARY_DIR}/libs/network")

# networkTest Executable
ADD_EXECUTABLE(networkTest ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(networkTest samsonNetworkStatic pa lm pthread ${COMMON_SAMSON_LIBS})

INSTALL(TARGETS               networkTest
        RUNTIME DESTINATION   bin)
