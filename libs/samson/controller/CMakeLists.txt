
SET (HEADERS
    AutomaticOperation.h
	AutomaticOperationManager.h
	ControllerDataManager.h
	ControllerTask.h
	ControllerTaskInfo.h
	ControllerTaskManager.h
	ControllerLoadManager.h
	Job.h
	JobManager.h
	Monitor.h
	MonitorParameter.h
	Queue.h
	QueueFile.h
	SamsonController.h
)

SET (SOURCES
    AutomaticOperation.cpp
    AutomaticOperationManager.cpp
    ControllerDataManager.cpp
    ControllerTask.cpp
    ControllerTaskInfo.cpp
    ControllerTaskManager.cpp
    ControllerLoadManager.cpp
    Job.cpp
    JobManager.cpp
    Monitor.cpp
    MonitorParameter.cpp
    Queue.cpp
    QueueFile.cpp
    SamsonController.cpp
)

# Inclue directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonControllerLib STATIC ${SOURCES} ${HEADERS} )









