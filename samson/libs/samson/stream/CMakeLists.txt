
SET (HEADERS
	BlockInfo.h
    BlockManager.h
    BlockList.h
    Block.h
	BlockLookupList.h
    StreamManager.h
	BlockListContainer.h
    StreamOperation.h
    QueueTaskManager.h
    Queue.h
	QueueConnections.h
	QueueTaskBase.h
    QueueTask.h
	SystemQueueTask.h
    StreamProcessBase.h
	PopQueueTask.h
	BlockBreakQueueTask.h
    PopQueue.h
    QueueTasks.h
	StreamOutput.h
	DistributionInformation.h
)


SET (SOURCES
	BlockInfo.cpp
    BlockManager.cpp
    BlockList.cpp
    Block.cpp
	BlockLookupList.cpp
    StreamManager.cpp
	BlockListContainer.cpp
    StreamOperation.cpp
    QueueTaskManager.cpp
    Queue.cpp
	QueueConnections.cpp
	QueueTaskBase.cpp
    QueueTask.cpp
	SystemQueueTask.cpp
    StreamProcessBase.cpp
	PopQueueTask.cpp
	BlockBreakQueueTask.cpp
    PopQueue.cpp
    QueueTasks.cpp
	StreamOutput.cpp
	DistributionInformation.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonStream STATIC ${SOURCES} ${HEADERS} )












