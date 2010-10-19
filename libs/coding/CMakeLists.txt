SET (HEADERS
    KVFileSaver.h
    KVWriterToBuffer.h
    coding.h
    BufferSinkInterface.h
)

SET (SOURCES
    KVFileSaver.cpp
    KVWriterToBuffer.cpp
    coding.cpp
)


SET (STATIC_LIBS
    samsonCommonStatic
    samsonMemoryStatic
)

SET (SHARED_LIBS
    samsonCommonShared
    samsonMemoryShared
)

include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")

ADD_LIBRARY(samsonCodingStatic STATIC ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonCodingStatic ${STATIC_LIBS})
SET_TARGET_PROPERTIES(samsonCodingStatic PROPERTIES OUTPUT_NAME samsonCoding)

# ADD_LIBRARY(samsonCodingShared SHARED ${SOURCES} ${HEADERS} )
# TARGET_LINK_LIBRARIES(samsonCodingShared ${SHARED_LIBS})
# SET_TARGET_PROPERTIES(samsonCodingShared PROPERTIES OUTPUT_NAME samsonCoding)





