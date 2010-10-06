SET (HEADERS
	AUCommandLine.h
	AUConsole.h
	ThreadStopPoint.h
	LockDebugger.h
	StopLock.h
	Lock.h
	KVManager.h
	KVManagerLogs.h
	KVQueue.h
	KVQueueWriter.h
	KVQueuesManager.h
	KVReader.h
	KVMultipleReader.h
	KVSet.h
	KVSetBuffer.h
	KVSetMaintainer.h
	KVSetsManager.h
	MemoryController.h
	ModulesManager.h
	NetworkManager.h
	ParameterMonitor.h
	SSLogger.h
	SSManager.h
	SSMonitor.h
	Sockets.h
	TaskBase.h
	TaskManager.h
	Task.h
	Buffer.h
	BufferSizes.h
	samsonTuning.h
	KvInfoData.h
	KvInfo.h
	KVQueueProcess.h
)

SET (SOURCES
	Lock.cpp
	LockDebugger.cpp
	KVManager.cpp
	KVManagerLogs.cpp
	KVQueue.cpp
	KVQueueWriter.cpp
	KVQueuesManager.cpp
	KVReader.cpp
	KVMultipleReader.cpp
	KVReaderCompare.h
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
	TaskBase.cpp
	TaskManager.cpp
	Task.cpp
	samson.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_BINARY_DIR}/libs/protocol")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")

ADD_LIBRARY(samsonCoreStatic STATIC ${SOURCES} ${HEADERS})
#ADD_LIBRARY(samsonCoreShared SHARED ${SOURCES} ${HEADERS})

#SET_TARGET_PROPERTIES(samsonCoreStatic samsonCoreShared PROPERTIES OUTPUT_NAME samsonCore)
SET_TARGET_PROPERTIES(samsonCoreStatic  PROPERTIES OUTPUT_NAME samsonCore)

TARGET_LINK_LIBRARIES(samsonCoreStatic samsonProtocolsStatic protobuf z curses pthread)
#TARGET_LINK_LIBRARIES(samsonCoreShared samsonProtocolsStatic protobuf z curses pthread)

#INSTALL(TARGETS samsonCoreStatic DESTINATION lib)
#INSTALL(TARGETS samsonCoreShared DESTINATION lib)
