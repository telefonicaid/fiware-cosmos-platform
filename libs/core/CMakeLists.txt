SET (HEADERS
Buffer.h
BufferVector.h
Engine.h
EngineNotification.h
EngineNotificationElement.h
EngineElement.h
MemoryManager.h
MemoryRequest.h
SharedMemoryManager.h
SharedMemoryItem.h
ModulesManager.h
ProcessManager.h
ProcessItem.h
ProcessItemIsolated.h
DiskManager.h
DiskOperation.h
DiskStatistics.h
)

SET (SOURCES
ModulesManager.cpp
Buffer.cpp
BufferVector.cpp
Engine.cpp
EngineNotification.cpp
EngineNotificationElement.cpp
EngineElement.cpp
MemoryManager.cpp
SharedMemoryManager.cpp
SharedMemoryItem.cpp
MemoryRequest.cpp
ProcessManager.cpp
ProcessItem.cpp
ProcessItemIsolated.cpp
DiskManager.cpp
DiskOperation.cpp
DiskStatistics.cpp
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



