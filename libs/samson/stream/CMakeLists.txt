
SET (HEADERS
    Block.h
    BlockManager.h
    Queue.h
)

SET (SOURCES
    Block.cpp
    BlockManager.cpp
    Queue.cpp
    
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/engine")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonStream STATIC ${SOURCES} ${HEADERS} )



