SET (HEADERS
	AUCommandLine.h
	AUConsole.h
	AULocker.h
	Data.h
	Factory.h
	Format.h
	KVFormat.h
	KVManager.h
	KVManagerLogs.h
	KVQueue.h
	KVQueueWriter.h
	KVQueuesManager.h
	KVReader.h
	KVSet.h
	KVSetBuffer.h
	KVSetMaintainer.h
	KVSetsManager.h
	MemoryController.h
	Module.h
	ModulesManager.h
	NetworkManager.h
	Operation.h
	ParameterMonitor.h
	SSLogger.h
	SSManager.h
	SSMonitor.h
	Sockets.h
	Task.h
	TaskManager.h
	Tasks.h
	au.h
	common.h
	samson.h
	samson_lib.h
)

SET (INSTALL_HEADERS
	Data.h
	KVFormat.h
	Module.h
	Operation.h
	samson_lib.h
)

SET (SOURCES
	AULocker.cpp
	KVManager.cpp
	KVManagerLogs.cpp
	KVQueue.cpp
	KVQueueWriter.cpp
	KVQueuesManager.cpp
	KVReader.cpp
	KVSet.cpp
	KVSetBuffer.cpp
	KVSetMaintainer.cpp
	KVSetsManager.cpp
	MemoryController.cpp
	ModulesManager.cpp
	NetworkManager.cpp
	SSLogger.cpp
	SSManager.cpp
	SSMonitor.cpp
	Sockets.cpp
	Task.cpp
	TaskManager.cpp
	Tasks.cpp
	au.cpp
	samson.cpp
)

include_directories("${PROJECT_BINARY_DIR}/libs/protocol")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")

ADD_LIBRARY(samsonCoreStatic STATIC ${SOURCES} ${HEADERS})
ADD_LIBRARY(samsonCoreShared SHARED ${SOURCES} ${HEADERS})

SET_TARGET_PROPERTIES(samsonCoreStatic samsonCoreShared PROPERTIES OUTPUT_NAME samsonCore)

TARGET_LINK_LIBRARIES(samsonCoreStatic z curses pthread)
TARGET_LINK_LIBRARIES(samsonCoreShared z curses pthread)


INSTALL(FILES ${INSTALL_HEADERS} DESTINATION include/samson)

INSTALL(TARGETS samsonCoreStatic DESTINATION lib)
INSTALL(TARGETS samsonCoreShared DESTINATION lib)
