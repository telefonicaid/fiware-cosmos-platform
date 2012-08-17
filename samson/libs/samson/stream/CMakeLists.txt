
SET (HEADERS
Block.h
BlockInfo.h
BlockList.h
BlockListContainer.h
BlockLookupList.h
BlockManager.h
StreamOperationInfo.h
WorkerSystemTask.h
WorkerTask.h
WorkerTaskBase.h
WorkerTaskManager.h
)

SET (SOURCES
Block.cpp
BlockInfo.cpp
BlockList.cpp
BlockListContainer.cpp
BlockLookupList.cpp
BlockManager.cpp
StreamOperationInfo.cpp
WorkerSystemTask.cpp
WorkerTask.cpp
WorkerTaskBase.cpp
WorkerTaskManager.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonStream STATIC ${SOURCES} ${HEADERS} )

