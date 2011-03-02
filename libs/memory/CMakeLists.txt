SET (HEADERS
    Buffer.h
    SimpleBuffer.h
    MemoryManager.h
)

SET (SOURCES
    Buffer.cpp
    MemoryManager.cpp
)

# Inclue directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonMemory STATIC ${SOURCES} ${HEADERS})
