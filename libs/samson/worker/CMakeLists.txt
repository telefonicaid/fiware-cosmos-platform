
SET (HEADERS
	SamsonWorker.h
)

SET (SOURCES
	SamsonWorker.cpp
)

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonWorkerLib STATIC ${SOURCES} ${HEADERS} )



