
SET (HEADERS
Block.h
BlockInfo.h
BlockRef.h
BlockList.h
BlockListContainer.h
BlockLookupList.h
BlockManager.h
StreamOperationGlobalInfo.h
StreamOperationRangeInfo.h
WorkerSystemTask.h
WorkerTask.h
WorkerTaskBase.h
WorkerTaskManager.h
BlockReaderCollection.h
)

SET (SOURCES
Block.cpp
BlockInfo.cpp
BlockRef.cpp
BlockList.cpp
BlockListContainer.cpp
BlockLookupList.cpp
BlockManager.cpp
StreamOperationGlobalInfo.cpp
StreamOperationRangeInfo.cpp
WorkerSystemTask.cpp
WorkerTask.cpp
WorkerTaskBase.cpp
WorkerTaskManager.cpp
BlockReaderCollection.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonStream STATIC ${SOURCES} ${HEADERS} )

