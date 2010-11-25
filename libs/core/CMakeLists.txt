
SET (HEADERS
    Monitor.h
    MonitorParameter.h
    DataBufferItem.h
    DataBufferItemDelegate.h
    DataBuffer.h
    ModulesManager.h
    SamsonWorker.h
    LoadDataManager.h
    WorkerTaskManager.h
    SamsonController.h
    ControllerDataManager.h
    JobManager.h
    Job.h
    ControllerTaskManager.h
    ControllerTask.h
    ControllerTaskInfo.h
    Queue.h
    QueueFile.h
    WorkerTask.h
    WorkerTaskItem.h
    WorkerTaskItemWithOutput.h
    WorkerTaskItemGenerator.h
    WorkerTaskItemParser.h
)

SET (SOURCES
    Monitor.cpp
    MonitorParameter.cpp
    DataBufferItem.cpp
    DataBufferItemDelegate.cpp
    DataBuffer.cpp
    ModulesManager.cpp
    SamsonWorker.cpp
    LoadDataManager.cpp
    WorkerTaskManager.cpp
    SamsonController.cpp
    ControllerDataManager.cpp
    JobManager.cpp
    ControllerTaskManager.cpp
    ControllerTask.cpp
    ControllerTaskInfo.cpp
    Job.cpp 
    Queue.cpp
    QueueFile.cpp
    WorkerTask.cpp
    WorkerTaskItem.cpp
    WorkerTaskItemWithOutput.cpp
    WorkerTaskItemGenerator.cpp
    WorkerTaskItemParser.cpp
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
    protobuf 
    dl 
    pthread
    ncurses
)

SET (SHARED_LIBS
    samsonNetworkShared 
    samsonModuleShared
    samsonCommonShared 
    samsonCodingShared
    samsonMemoryShared
    samsonNetworkShared 
    samsonDiskShared
    samsonProcessShared
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



ADD_LIBRARY(samsonCoreStatic STATIC ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( samsonCoreStatic ${STATIC_LIBS} )
SET_TARGET_PROPERTIES(samsonCoreStatic PROPERTIES OUTPUT_NAME samsonCore)

# ADD_LIBRARY(samsonCoreShared SHARED ${SOURCES} ${HEADERS} )
# TARGET_LINK_LIBRARIES(samsonCoreShared ${SHARED_LIBS} )
# SET_TARGET_PROPERTIES(samsonCoreShared PROPERTIES OUTPUT_NAME samsonCore)





