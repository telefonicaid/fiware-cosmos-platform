
SET (HEADERS
    Block.h
    BlockManager.h
    Queue.h
    QueuesManager.h
    QueueTask.h
    QueueTaskManager.h
)

SET (SOURCES
    Block.cpp
    BlockManager.cpp
    Queue.cpp
    QueuesManager.cpp
    QueueTask.cpp
    QueueTaskManager.cpp    
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonStream STATIC ${SOURCES} ${HEADERS} )











