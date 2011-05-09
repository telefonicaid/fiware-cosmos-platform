SET (HEADERS
Buffer.h
SimpleBuffer.h
Engine.h
EngineNotification.h
EngineNotificationElement.h
EngineElement.h
MemoryManager.h
MemoryRequest.h
ProcessManager.h
ProcessItem.h
ReadFile.h
ReadFileManager.h
DiskManager.h
DiskOperation.h
DiskStatistics.h
)

SET (SOURCES
Buffer.cpp
Engine.cpp
EngineNotification.cpp
EngineNotificationElement.cpp
EngineElement.cpp
MemoryManager.cpp
MemoryRequest.cpp
ProcessManager.cpp
ProcessItem.cpp
ReadFile.cpp
ReadFileManager.cpp
DiskManager.cpp
DiskOperation.cpp
DiskStatistics.cpp
)

# Inclue directories
# -----------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(engine STATIC ${SOURCES} ${HEADERS} )



