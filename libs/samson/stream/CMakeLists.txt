
SET (HEADERS
    Block.h
    BlockManager.h
    Queue.h
    QueuesManager.h
    QueueTask.h
    QueueTaskManager.h
    StreamProcessBase.h
    ParserQueueTask.h
)

SET (SOURCES
    Block.cpp
    BlockManager.cpp
    Queue.cpp
    QueuesManager.cpp
    QueueTask.cpp
    QueueTaskManager.cpp    
    StreamProcessBase.cpp
    ParserQueueTask.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonStream STATIC ${SOURCES} ${HEADERS} )











