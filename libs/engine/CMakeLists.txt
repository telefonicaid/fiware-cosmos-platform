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
DiskManager.cpp
DiskOperation.cpp
DiskStatistics.cpp
)

# Inclue directories
# -----------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(engine STATIC ${SOURCES} ${HEADERS} )



