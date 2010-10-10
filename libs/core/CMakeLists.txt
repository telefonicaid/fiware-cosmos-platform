SET (HEADERS
SamsonWorker.h
SamsonController.h
)

SET (SOURCES
SamsonWorker.cpp
SamsonController.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")

ADD_LIBRARY(samsonCoreStatic STATIC ${SOURCES} ${HEADERS} )
ADD_LIBRARY(samsonCoreShared SHARED ${SOURCES} ${HEADERS} )

TARGET_LINK_LIBRARIES( samsonCoreStatic samsonNetworkStatic protobuf )
TARGET_LINK_LIBRARIES( samsonCoreShared samsonNetworkStatic protobuf )

SET_TARGET_PROPERTIES(samsonCoreShared PROPERTIES OUTPUT_NAME samsonCore)
SET_TARGET_PROPERTIES(samsonCoreStatic PROPERTIES OUTPUT_NAME samsonCore)



