SET (HEADERS
    ModulesManager.h
    SamsonWorker.h
    LoadDataManager.h
    WorkerQueue.h
    WorkerTaskManager.h
    SamsonController.h
    ControllerDataManager.h
    ControllerQueue.h
    JobManager.h
    Job.h
    ControllerTaskManager.h
    ControllerTask.h
)

SET (SOURCES
    ModulesManager.cpp
    SamsonWorker.cpp
    LoadDataManager.cpp
    WorkerQueue.cpp
    WorkerTaskManager.cpp
    SamsonController.cpp
    ControllerDataManager.cpp
    ControllerQueue.cpp
    Job.cpp
    JobManager.cpp
    ControllerTaskManager.cpp
    ControllerTask.cpp
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

