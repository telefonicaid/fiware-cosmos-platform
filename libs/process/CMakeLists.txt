SET (HEADERS
    ProcessAssistant.h
    Process.h
)

SET (SOURCES
    ProcessAssistant.cpp
    Process.cpp
)

SET (STATIC_LIBS
    samsonCommonStatic
    pthread
)

include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")

link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/process")
link_directories("${PROJECT_BINARY_DIR}/libs/network")

ADD_LIBRARY(samsonProcessStatic STATIC ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( samsonProcessStatic ${STATIC_LIBS} )
SET_TARGET_PROPERTIES(samsonProcessStatic PROPERTIES OUTPUT_NAME samsonProcess)


