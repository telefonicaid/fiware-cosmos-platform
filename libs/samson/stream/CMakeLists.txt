
SET (HEADERS
    Block.h
    BlockManager.h
    BlockList.h
    Queue.h
    QueueItem.h
    StreamManager.h
	QueueTaskBase.h
    QueueTask.h
	SystemQueueTask.h
	PopQueueTask.h
	BlockBreakQueueTask.h
    QueueTaskManager.h
    StreamProcessBase.h
    PopQueue.h
    PopQueueManager.h
    StatesManager.h
    StreamOperation.h
    ParserQueueTask.h
	WorkerTask.h

)


SET (SOURCES
    Block.cpp
    BlockManager.cpp
    Queue.cpp
    QueueItem.cpp
    BlockList.cpp
    StreamManager.cpp
	QueueTaskBase.cpp
    QueueTask.cpp
	SystemQueueTask.cpp
	PopQueueTask.cpp
	BlockBreakQueueTask.cpp
    QueueTaskManager.cpp    
    StreamProcessBase.cpp
    PopQueueManager.cpp
    PopQueue.cpp
    StatesManager.cpp
    StreamOperation.cpp
    ParserQueueTask.cpp
	WorkerTask.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonStream STATIC ${SOURCES} ${HEADERS} )












