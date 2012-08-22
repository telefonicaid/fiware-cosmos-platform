
SET (HEADERS
	WorkerCommandManager.h
	WorkerCommand.h
	SamsonWorker.h
	PushManager.h
	DistributionBlocks.h
	SamsonWorkerSamples.h
	SamsonWorkerRest.h
)

SET (SOURCES
	WorkerCommandManager.cpp
	WorkerCommand.cpp
	SamsonWorker.cpp
	PushManager.cpp
	DistributionBlocks.cpp
	SamsonWorkerSamples.cpp
	SamsonWorkerRest.cpp
)

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("/usr/local/include/json")
include_directories("/usr/include/json")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonWorkerLib STATIC ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES( samsonWorkerLib ${QT_LIBRARIES} json )


