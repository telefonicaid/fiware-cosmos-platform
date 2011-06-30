SET (HEADERS
Object.h
ObjectsManager.h
Buffer.h
SimpleBuffer.h
Engine.h
Notification.h
NotificationSystem.h
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
EngineOperationsContainer.h	
)

SET (SOURCES
Object.cpp
ObjectsManager.cpp
Buffer.cpp
Engine.cpp
Notification.cpp
NotificationSystem.cpp
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
EngineOperationsContainer.cpp
)

# Inclue directories
# -----------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(engine STATIC ${SOURCES} ${HEADERS} )



