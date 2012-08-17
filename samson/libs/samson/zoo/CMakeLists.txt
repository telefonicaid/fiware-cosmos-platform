SET (HEADERS
	Connection.h
	ConnectionWatcherInterface.h
	ZooNodeCommiter.h
	SamsonWorkerController.h
	DataModel.h
	CommitCommand.h
	common.h
)

SET (SOURCES
	Connection.cpp
	ZooNodeCommiter.cpp
	SamsonWorkerController.cpp
	DataModel.cpp
	CommitCommand.cpp
	common.cpp
)

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonZoo STATIC ${SOURCES} ${HEADERS})



