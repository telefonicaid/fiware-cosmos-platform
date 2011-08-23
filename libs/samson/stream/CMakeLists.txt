
SET (HEADERS
    Block.h
    BlockManager.h
    BlockMatrix.h
    BlockList.h
    Queue.h
    QueuesManager.h
    QueueTask.h
    QueueTaskManager.h
    StreamProcessBase.h
    ParserQueueTask.h
    PopQueue.h
    PopQueueManager.h
    StatesManager.h
    State.h
    StateItem.h
    StreamOperation.h
)

SET (SOURCES
    Block.cpp
    BlockManager.cpp
    Queue.cpp
    BlockMatrix.cpp
    BlockList.cpp
    QueuesManager.cpp
    QueueTask.cpp
    QueueTaskManager.cpp    
    StreamProcessBase.cpp
    ParserQueueTask.cpp
    PopQueueManager.cpp
    PopQueue.cpp
    StatesManager.cpp
    State.cpp
    StateItem.cpp
    StreamOperation.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonStream STATIC ${SOURCES} ${HEADERS} )












