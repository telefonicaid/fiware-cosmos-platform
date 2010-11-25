SET (HEADERS
    ProcessCommunication.h
    ProcessAssistantBase.h
    ProcessBase.h
    ProcessAssistant.h
    Process.h
    ProcessWriter.h
    OperationFramework.h
    ProcessOperationFramework.h
    ProcessAssistantOperationFramework.h
)

SET (SOURCES
    ProcessCommunication.cpp
    ProcessAssistantBase.cpp
    ProcessBase.cpp
    ProcessAssistant.cpp
    Process.cpp
    ProcessWriter.cpp
    OperationFramework.cpp
    ProcessOperationFramework.cpp
    ProcessAssistantOperationFramework.cpp
)

SET (STATIC_LIBS
    samsonCommonStatic
    pthread
)

include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/disk")
include_directories("${PROJECT_SOURCE_DIR}/libs/coding")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")

link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/process")
link_directories("${PROJECT_BINARY_DIR}/libs/network")

ADD_LIBRARY(samsonProcessStatic STATIC ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( samsonProcessStatic ${STATIC_LIBS} )
SET_TARGET_PROPERTIES(samsonProcessStatic PROPERTIES OUTPUT_NAME samsonProcess)


