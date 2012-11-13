
SET (HEADERS
	WorkerCommandManager.h
	WorkerCommand.h
	SamsonWorker.h
	WorkerBlockManager.h
	BlockRequest.h
	SamsonWorkerSamples.h
	SamsonWorkerRest.h
	DataModel.h
	SamsonWorkerController.h
	CommitCommand.h
)

SET (SOURCES
	WorkerCommandManager.cpp
	WorkerCommand.cpp
	SamsonWorker.cpp
	WorkerBlockManager.cpp
	BlockRequest.cpp
	SamsonWorkerSamples.cpp
	SamsonWorkerRest.cpp
	DataModel.cpp
	SamsonWorkerController.cpp
	CommitCommand.cpp
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



