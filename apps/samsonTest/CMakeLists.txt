SET (HEADERS
)

SET (SOURCES
)

SET (STATIC_LIBS
    samsonCoreStatic 
    samsonNetworkStatic 
    samsonModuleStatic 
    samsonCommonStatic 
    samsonCodingStatic
    samsonMemoryStatic
    samsonNetworkStatic 
    samsonProcessStatic
    samsonDiskStatic
    samsonDataStatic
    protobuf 
    dl 
    pthread
    ncurses
)


include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/coding")
include_directories("${PROJECT_SOURCE_DIR}/libs/disk")
include_directories("${PROJECT_SOURCE_DIR}/libs/data")
include_directories("${PROJECT_SOURCE_DIR}/libs/process")

link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")

ADD_EXECUTABLE(samsonTest main_samsonTest.cpp ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonTest ${STATIC_LIBS} )

INSTALL(TARGETS               samsonTest
        RUNTIME DESTINATION   bin)
