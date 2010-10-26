SET (HEADERS
    ProcessAssitant.h
    Process.h
)

SET (SOURCES
    ProcessAssitant.cpp
    Process.cpp
)

SET (STATIC_LIBS
    samsonCommonStatic 
    pthread
)

include_directories("${PROJECT_SOURCE_DIR}/libs/common")


ADD_LIBRARY(samsonProcessStatic STATIC ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( samsonProcessStatic ${STATIC_LIBS} )
SET_TARGET_PROPERTIES(samsonProcessStatic PROPERTIES OUTPUT_NAME samsonProcess)


