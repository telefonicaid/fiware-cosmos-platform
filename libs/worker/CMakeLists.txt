
SET (HEADERS
	BufferVector.h
	DataBufferProcessItem.h
	SamsonWorker.h
	LoadDataManager.h
	WorkerTaskManager.h
	WorkerTask.h
	WorkerSubTask.h
	ProcessBase.h
	ProcessGenerator.h
	ProcessCompact.h
	ProcessOperation.h
	ProcessParser.h
	ProcessWriter.h
	KVInputVector.h
	ProcessItemIsolated.h
	SharedMemoryManager.h
	SharedMemoryItem.h
)

SET (SOURCES
	BufferVector.cpp
	DataBufferProcessItem.cpp
	SamsonWorker.cpp
	LoadDataManager.cpp
	WorkerTaskManager.cpp
	WorkerTask.cpp
        WorkerSubTask.cpp
        ProcessBase.cpp
	ProcessGenerator.cpp
	ProcessCompact.cpp
	ProcessOperation.cpp
	ProcessParser.cpp
	ProcessWriter.cpp
	KVInputVector.cpp
	ProcessItemIsolated.cpp
	SharedMemoryManager.cpp
	SharedMemoryItem.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/coding")
include_directories("${PROJECT_SOURCE_DIR}/libs/disk")
include_directories("${PROJECT_SOURCE_DIR}/libs/data")
include_directories("${PROJECT_SOURCE_DIR}/libs/process")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonWorkerLib STATIC ${SOURCES} ${HEADERS} )



