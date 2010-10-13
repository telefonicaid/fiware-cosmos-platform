SET (HEADERS
    samsonDirectories.h
    ModulesManager.h
)

SET (SOURCES
    ModulesManager.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")

ADD_LIBRARY(samsonCoreStatic STATIC ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( samsonCoreStatic samsonNetworkStatic samsonCommonStatic protobuf )
SET_TARGET_PROPERTIES(samsonCoreStatic PROPERTIES OUTPUT_NAME samsonCore)

ADD_LIBRARY(samsonCoreShared SHARED ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonCoreShared samsonNetworkShared samsonCommonShared protobuf )
SET_TARGET_PROPERTIES(samsonCoreShared PROPERTIES OUTPUT_NAME samsonCore)

ADD_EXECUTABLE(samsonTest main.cpp ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonTest samsonNetworkStatic samsonCommonStatic samsonCodingStatic protobuf dl pthread)
