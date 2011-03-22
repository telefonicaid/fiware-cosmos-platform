SET (SOURCES
    main_SamsonCheck.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     samsonCore
     samsonNetwork
     samsonCommon
     protobuf
     pthread
     dl
     lm
     ${COMMON_SAMSON_LIBS}
)


# Include directories
# -----------------------------------------------------------
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



# SAMSON Executable
# -----------------------------------------------------------

ADD_EXECUTABLE(samsonCheck ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonCheck ${STATIC_LIBS})

INSTALL(TARGETS               samsonCheck
        RUNTIME DESTINATION   bin)

