SET (SOURCES
    main_SamsonSetup.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     samsonNetworkStatic
     samsonCoreStatic
     samsonCommonStatic
     protobuf
     pthread
     dl
     lm
)


include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/data")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/delilah")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/coding
include_directories("${PROJECT_SOURCE_DIR}/libs/disk")")
include_directories("${PROJECT_SOURCE_DIR}/libs/process")


link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")


# SAMSON Executable
ADD_EXECUTABLE(samsonSetup ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonSetup ${STATIC_LIBS})

INSTALL(TARGETS               samsonSetup
        RUNTIME DESTINATION   bin)

