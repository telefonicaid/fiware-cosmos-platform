SET (SOURCES
    main_SamsonDemo.cpp
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
     samsonNetworkStatic
     samsonCoreStatic
     samsonCommonStatic
     delilahStatic
     lm
     protobuf
     pthread
     dl
     curses
)

SET (RESOURCES
    ${PROJECT_SOURCE_DIR}/libs/delilah/resources/objects.qrc
    ${PROJECT_SOURCE_DIR}/libs/delilah/resources/icons.qrc)

QT4_ADD_RESOURCES(RESOURCES_QRC ${RESOURCES})


include_directories("/usr/include/qt4")

include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/data")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/delilah")

link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")


# SAMSON Executable
ADD_EXECUTABLE(samsonDemo ${SOURCES} ${HEADERS} ${RESOURCES_QRC})
TARGET_LINK_LIBRARIES(samsonDemo ${STATIC_LIBS})

INSTALL(TARGETS               samsonDemo
        RUNTIME DESTINATION   bin)

