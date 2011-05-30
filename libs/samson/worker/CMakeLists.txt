
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
	KVInputVector.h
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
	KVInputVector.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonWorkerLib STATIC ${SOURCES} ${HEADERS} )



