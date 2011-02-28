
SET (HEADERS
	DataBufferProcessItem.h
	SamsonWorker.h
	LoadDataManager.h
	WorkerTaskManager.h
	WorkerTask.h
	WorkerSubTask.h
	ProcessBase.h
	ProcessGenerator.h
	ProcessOrganizer.h
	ProcessOperation.h
	ProcessParser.h
	ProcessParserOut.h
	ProcessWriter.h
)

SET (SOURCES
	DataBufferProcessItem.cpp
	SamsonWorker.cpp
	LoadDataManager.cpp
	WorkerTaskManager.cpp
	WorkerTask.cpp
        WorkerSubTask.cpp
        ProcessBase.cpp
	ProcessGenerator.cpp
	ProcessOrganizer.cpp
	ProcessOperation.cpp
	ProcessParser.cpp
	ProcessParserOut.cpp
	ProcessWriter.cpp
)

# Include directories
# -----------------------------------------------------------------
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
include_directories("${PROJECT_SOURCE_DIR}/libs/core")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonWorkerLib STATIC ${SOURCES} ${HEADERS} )



