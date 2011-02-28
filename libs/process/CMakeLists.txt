SET (HEADERS
    ProcessManager.h
    ProcessItem.h
    ProcessItemIsolated.h
)

SET (SOURCES
    ProcessManager.cpp
    ProcessItem.cpp
    ProcessItemIsolated.cpp
)

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/disk")
include_directories("${PROJECT_SOURCE_DIR}/libs/coding")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/worker")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonProcess STATIC ${SOURCES} ${HEADERS} )


