SET (HEADERS
    ModulesManager.h
    Delilah.h
    DelilahConsole.h
    SamsonWorker.h
    WorkerDataManager.h
    WorkerTaskManager.h
    SamsonController.h
    ControllerDataManager.h
    ControllerTaskManager.h
    ControllerTask.h
    DataManager.h
    data.pb.h
)

SET (SOURCES
    ModulesManager.cpp
    Delilah.cpp
    DelilahConsole.cpp
    SamsonWorker.cpp
    WorkerDataManager.cpp
    WorkerTaskManager.cpp
    SamsonController.cpp
    ControllerDataManager.cpp
    ControllerTaskManager.cpp
    ControllerTask.cpp
    DataManager.cpp
    data.pb.cc
)

SET (STATIC_LIBS
    samsonNetworkStatic 
    samsonModuleStatic 
    samsonCommonStatic 
    samsonCodingStatic
    samsonMemoryStatic
    samsonNetworkStatic 
    samsonDiskStatic
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


# Generate 'data.pb.h' and 'data.pb.cc' from ''data.proto''
add_custom_command (
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/data.pb.cc
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/data.pb.h
  COMMAND /usr/local/bin/protoc ${CMAKE_CURRENT_SOURCE_DIR}/data.proto --cpp_out=${CMAKE_CURRENT_SOURCE_DIR} --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
  MAIN_DEPENDENCY data.proto
)

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
