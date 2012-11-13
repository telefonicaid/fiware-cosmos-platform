SET (HEADERS
Logs.h
NotificationListener.h
NotificationListenersManager.h
Buffer.h
SimpleBuffer.h
EngineElementCollection.h
Engine.h
Notification.h
NotificationElement.h
EngineElement.h
MemoryManager.h
ProcessManager.h
ProcessItem.h
ReadFile.h
ReadFileManager.h
DiskManager.h
DiskOperation.h
)

SET (SOURCES
Logs.cpp
NotificationListener.cpp
NotificationListenersManager.cpp
Buffer.cpp
EngineElementCollection.cpp
Engine.cpp
Notification.cpp
NotificationElement.cpp
EngineElement.cpp
MemoryManager.cpp
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
ADD_LIBRARY(engine SHARED ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( engine  lm pa au )
INSTALL( TARGETS engine DESTINATION lib )


