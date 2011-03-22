
SET (HEADERS
Buffer.h
BufferVector.h
DiskOperation.h
DiskStatistics.h
EngineDelegates.h
Engine.h
EngineElement.h
MemoryManager.h
MemoryRequest.h
ModulesManager.h
ProcessItem.h
ProcessItemIsolated.h
)

SET (SOURCES
Buffer.cpp
BufferVector.cpp
DiskOperation.cpp
DiskStatistics.cpp
Engine.cpp
EngineElement.cpp
MemoryManager.cpp
MemoryRequest.cpp
ModulesManager.cpp
ProcessItem.cpp
ProcessItemIsolated.cpp
)

# Inclue directories
# -----------------------------------------------------------------
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

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonCore STATIC ${SOURCES} ${HEADERS} )



