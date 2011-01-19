
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



