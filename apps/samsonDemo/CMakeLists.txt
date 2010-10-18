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


include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")

# SAMSON Executable
ADD_EXECUTABLE(samsonDemo ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonDemo samsonNetworkStatic samsonCoreStatic samsonCommonStatic lm protobuf pthread dl curses)

INSTALL(TARGETS               samsonDemo
        RUNTIME DESTINATION   bin)

