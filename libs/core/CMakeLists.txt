SET (HEADERS
    ModulesManager.h
    Delilah.h
    DelilahConsole.h
    SamsonWorker.h
    WorkerDataManager.h
    WorkerQueue.h
    WorkerTaskManager.h
    DataBuffer.h
    SamsonController.h
    ControllerDataManager.h
    ControllerQueue.h
    ControllerTaskManager.h
    ControllerTask.h
)

SET (SOURCES
    ModulesManager.cpp
    Delilah.cpp
    DelilahConsole.cpp
    SamsonWorker.cpp
    WorkerDataManager.cpp
    WorkerQueue.cpp
    WorkerTaskManager.cpp
    DataBuffer.cpp
    SamsonController.cpp
    ControllerDataManager.cpp
    ControllerQueue.cpp
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



ADD_LIBRARY(samsonCoreStatic STATIC ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( samsonCoreStatic ${STATIC_LIBS} )
SET_TARGET_PROPERTIES(samsonCoreStatic PROPERTIES OUTPUT_NAME samsonCore)

# ADD_LIBRARY(samsonCoreShared SHARED ${SOURCES} ${HEADERS} )
# TARGET_LINK_LIBRARIES(samsonCoreShared ${SHARED_LIBS} )
# SET_TARGET_PROPERTIES(samsonCoreShared PROPERTIES OUTPUT_NAME samsonCore)

ADD_EXECUTABLE(samsonTest main.cpp ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonTest ${STATIC_LIBS} )

ADD_EXECUTABLE(samsonMemoryTest memoryTest.cpp ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonMemoryTest  ${STATIC_LIBS} )
INSTALL(TARGETS               samsonMemoryTest
        RUNTIME DESTINATION   bin)

ADD_EXECUTABLE(samsonDiskTest samsonDiskTest.cpp ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonDiskTest  ${STATIC_LIBS} )
INSTALL(TARGETS               samsonDiskTest
        RUNTIME DESTINATION   bin)
