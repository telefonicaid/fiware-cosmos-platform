
SET (HEADERS
    SharedMemoryItem.h
    SharedMemoryManager.h
    ProcessItemIsolated.h
    ProcessIsolated.h
    ProcessWriter.h
)

SET (SOURCES
    SharedMemoryItem.cpp
    SharedMemoryManager.cpp
    ProcessItemIsolated.cpp
    ProcessIsolated.cpp
    ProcessWriter.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonIsolated STATIC ${SOURCES} ${HEADERS} )

