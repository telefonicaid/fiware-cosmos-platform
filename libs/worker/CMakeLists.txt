
SET (HEADERS
    DataBuffer.h
    DataBufferItem.h
	LoadDataManager.h
	SamsonWorker.h
	WorkerTask.h
	WorkerTaskItem.h
	WorkerTaskItemGenerator.h
	WorkerTaskItemOperation.h
	WorkerTaskItemOrganizer.h
	WorkerTaskItemParser.h
	WorkerTaskManager.h
)

SET (SOURCES
    DataBuffer.cpp
	DataBufferItem.cpp
	LoadDataManager.cpp
	SamsonWorker.cpp
	WorkerTask.cpp
	WorkerTaskItem.cpp
	WorkerTaskItemGenerator.cpp
	WorkerTaskItemOperation.cpp
	WorkerTaskItemOrganizer.cpp
	WorkerTaskItemParser.cpp
	WorkerTaskManager.cpp
)

SET (STATIC_LIBS
    samsonNetworkStatic 
    samsonModuleStatic 
    samsonCommonStatic 
    samsonCodingStatic
    samsonMemoryStatic
    samsonNetworkStatic 
    samsonDiskStatic
    samsonDataStatic
    samsonProcessStatic
    samsonCoreStatic
    protobuf 
    dl 
    pthread
    ncurses
)

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



ADD_LIBRARY(samsonWorkerStatic STATIC ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( samsonWorkerStatic ${STATIC_LIBS} )
SET_TARGET_PROPERTIES(samsonWorkerStatic PROPERTIES OUTPUT_NAME samsonWorker)



