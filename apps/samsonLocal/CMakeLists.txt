SET (SOURCES
    main_samsonLocal.cpp
    FakeEndpoint.cpp
    NetworkFake.cpp
    NetworkFakeCenter.ccp
)

SET (HEADERS
    FakeEndpoint.h
    NetworkFake.h
    NetworkFakeCenter.h
)

SET( STATIC_LIBS
     samsonWorkerStatic
     samsonModuleStatic
     samsonProcessStatic
     samsonNetworkStatic
     samsonCommonStatic
     samsonCoreStatic
     samsonControllerStatic
     delilahStatic
     pa
     lm
     protobuf
     pthread
     dl
     readline
)

SET (RESOURCES
    ${PROJECT_SOURCE_DIR}/libs/delilah/resources/objects.qrc
    ${PROJECT_SOURCE_DIR}/libs/delilah/resources/icons.qrc)


include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/data")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/delilah")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/coding
include_directories("${PROJECT_SOURCE_DIR}/libs/disk")")
include_directories("${PROJECT_SOURCE_DIR}/libs/process")
include_directories("${PROJECT_SOURCE_DIR}/libs/controller")
include_directories("${PROJECT_SOURCE_DIR}/libs/worker")

link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/parseArgs")


# SAMSON Executable
ADD_EXECUTABLE(samsonLocal ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonLocal ${STATIC_LIBS})

INSTALL(TARGETS               samsonLocal
        RUNTIME DESTINATION   bin)

