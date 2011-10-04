
SET (HEADERS
    BlockManager.h
    BlockList.h
    Block.h
    StreamManager.h
    QueueTaskManager.h
    Queue.h
	QueueTaskBase.h
    QueueTask.h
	SystemQueueTask.h
    StreamProcessBase.h
	PopQueueTask.h
	BlockBreakQueueTask.h
    PopQueue.h
    QueueTasks.h
	WorkerCommand.h
)


SET (SOURCES
    BlockManager.cpp
    BlockList.cpp
    Block.cpp
    StreamManager.cpp
    QueueTaskManager.cpp
    Queue.cpp
	QueueTaskBase.cpp
    QueueTask.cpp
	SystemQueueTask.cpp
    StreamProcessBase.cpp
	PopQueueTask.cpp
	BlockBreakQueueTask.cpp
    PopQueue.cpp
    QueueTasks.cpp
	WorkerCommand.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonStream STATIC ${SOURCES} ${HEADERS} )












