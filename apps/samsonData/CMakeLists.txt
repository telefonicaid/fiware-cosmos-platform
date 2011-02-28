SET (SOURCES
    main_SamsonData.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     samsonNetwork
     samsonCore
     samsonCommon
     samsonDataLib
     delilahLib
     protobuf
     pthread
     dl
     lm
)

# Include directories
# ------------------------------------------------------------

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

# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonData ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonData ${STATIC_LIBS})

INSTALL(TARGETS               samsonData
        RUNTIME DESTINATION   bin)

