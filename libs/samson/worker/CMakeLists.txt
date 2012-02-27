
SET (HEADERS
	WorkerCommandManager.h
	WorkerCommand.h
	SamsonWorker.h
)

SET (SOURCES
	WorkerCommandManager.cpp
	WorkerCommand.cpp
	SamsonWorker.cpp
)

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonWorkerLib STATIC ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( samsonWorkerLib ${QT_LIBRARIES} )


