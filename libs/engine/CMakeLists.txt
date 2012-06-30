SET (HEADERS
Object.h
ObjectsManager.h
Buffer.h
BufferContainer.h
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
ConnectionsManager.h
Connection.h
)

SET (SOURCES
Object.cpp
ObjectsManager.cpp
Buffer.cpp
BufferContainer.cpp
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
ConnectionsManager.cpp
Connection.cpp
)

# Inclue directories
# -----------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(engine STATIC ${SOURCES} ${HEADERS} )



