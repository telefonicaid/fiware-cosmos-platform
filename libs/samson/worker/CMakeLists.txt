
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

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonWorkerLib STATIC ${SOURCES} ${HEADERS} )



