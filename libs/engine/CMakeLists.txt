SET (HEADERS
Object.h
ObjectsManager.h
Buffer.h
SimpleBuffer.h
Engine.h
EngineService.h
Notification.h
NotificationElement.h
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
OperationsContainer.h	
)

SET (SOURCES
Object.cpp
ObjectsManager.cpp
Buffer.cpp
Engine.cpp
EngineService.cpp
Notification.cpp
NotificationElement.cpp
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
OperationsContainer.cpp
)

# Inclue directories
# -----------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(engine STATIC ${SOURCES} ${HEADERS} )



