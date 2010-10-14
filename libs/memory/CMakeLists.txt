SET (HEADERS
    Buffer.h
    MemoryManager.h
)

SET (SOURCES
    Buffer.cpp
    MemoryManager.cpp
)

SET( LIBS_STATIC
     samsonCommonStatic
     protobuf
     ncurses
)

SET( LIBS_SHARED
     samsonCommonShared
     protobuf
     ncurses
)

include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")


ADD_LIBRARY(samsonMemoryStatic STATIC ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonMemoryStatic ${LIBS_STATIC})
SET_TARGET_PROPERTIES(samsonMemoryStatic PROPERTIES OUTPUT_NAME samsonMemory)

ADD_LIBRARY(samsonMemoryShared SHARED ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonMemoryShared  ${LIBS_SHARED} )
SET_TARGET_PROPERTIES(samsonMemoryShared PROPERTIES OUTPUT_NAME samsonMemory)


ADD_EXECUTABLE(samsonMemoryTest test.cpp ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonMemoryTest  ${LIBS_STATIC} pthread)
INSTALL(TARGETS               samsonMemoryTest
        RUNTIME DESTINATION   bin)
