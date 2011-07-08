
SET (HEADERS
    Block.h
    BlockManager.h
    BlockMatrix.h
    Queue.h
    QueuesManager.h
    QueueTask.h
    QueueTaskManager.h
    StreamProcessBase.h
    ParserQueueTask.h
    PopQueue.h
    PopQueueManager.h
)

SET (SOURCES
    Block.cpp
    BlockManager.cpp
    Queue.cpp
    BlockMatrix.cpp
    QueuesManager.cpp
    QueueTask.cpp
    QueueTaskManager.cpp    
    StreamProcessBase.cpp
    ParserQueueTask.cpp
    PopQueueManager.cpp
    PopQueue.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonStream STATIC ${SOURCES} ${HEADERS} )












