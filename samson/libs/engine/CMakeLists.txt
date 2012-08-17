SET (HEADERS
Object.h
ObjectsManager.h
Buffer.h
SimpleBuffer.h
EngineElementCollection.h
Engine.h
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
)

SET (SOURCES
Object.cpp
ObjectsManager.cpp
Buffer.cpp
EngineElementCollection.cpp
Engine.cpp
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
)

# Inclue directories
# -----------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(engine STATIC ${SOURCES} ${HEADERS} )



