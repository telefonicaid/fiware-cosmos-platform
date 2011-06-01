SET (HEADERS
	Host.h
	HostMgr.h
	Network2.h
	JobQueue.h
	EndpointManager.h
	Endpoint2.h
	ListenerEndpoint.h
	UnhelloedEndpoint.h
	WebListenerEndpoint.h
	WebWorkerEndpoint.h
	WorkerEndpoint.h
	ControllerEndpoint.h
	SpawnerEndpoint.h
	DelilahEndpoint.h
)

SET (SOURCES
	HostMgr.cpp
	Network2.cpp
	JobQueue.cpp
	EndpointManager.cpp
	Endpoint2.cpp
	ListenerEndpoint.cpp
	UnhelloedEndpoint.cpp
	WebListenerEndpoint.cpp
	WebWorkerEndpoint.cpp
	WorkerEndpoint.cpp
	ControllerEndpoint.cpp
	SpawnerEndpoint.cpp
	DelilahEndpoint.cpp
)

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonNetwork STATIC ${SOURCES} ${HEADERS} )

