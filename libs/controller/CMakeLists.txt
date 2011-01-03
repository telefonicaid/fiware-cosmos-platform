
SET (HEADERS
    AutomaticOperation.h
	AutomaticOperationManager.h
	ControllerDataManager.h
	ControllerTask.h
	ControllerTaskInfo.h
	ControllerTaskManager.h
	Job.h
	JobManager.h
	Monitor.h
	MonitorParameter.h
	Queue.h
	QueueFile.h
	SamsonController.h
)

SET (SOURCES
    AutomaticOperation.cpp
    AutomaticOperationManager.cpp
    ControllerDataManager.cpp
    ControllerTask.cpp
    ControllerTaskInfo.cpp
    ControllerTaskManager.cpp
    Job.cpp
    JobManager.cpp
    Monitor.cpp
    MonitorParameter.cpp
    Queue.cpp
    QueueFile.cpp
    SamsonController.cpp
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



ADD_LIBRARY(samsonControllerStatic STATIC ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( samsonControllerStatic ${STATIC_LIBS} )
SET_TARGET_PROPERTIES(samsonControllerStatic PROPERTIES OUTPUT_NAME samsonCore)









