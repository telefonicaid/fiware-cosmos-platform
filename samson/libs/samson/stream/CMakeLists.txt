
SET (HEADERS
Block.h
BlockInfo.h
BlockList.h
BlockListContainer.h
BlockLookupList.h
BlockManager.h
BlockReaderCollection.h
BlockRef.h
ProcessIsolated.h
ProcessItemIsolated.h
ProcessWriter.h
SharedMemoryItem.h
SharedMemoryManager.h
StreamOperationGlobalInfo.h
StreamOperationRangeInfo.h
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
BlockReaderCollection.cpp
BlockRef.cpp
ProcessIsolated.cpp
ProcessItemIsolated.cpp
ProcessWriter.cpp
SharedMemoryItem.cpp
SharedMemoryManager.cpp
StreamOperationGlobalInfo.cpp
StreamOperationRangeInfo.cpp
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



