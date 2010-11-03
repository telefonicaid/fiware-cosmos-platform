SET (HEADERS
	samsonDirectories.h
	samsonLogMsg.h
	CommandLine.h
	Console.h
	Format.h
	LockDebugger.h
	Lock.h
	StopLock.h
	SamsonSetup.h
	traces.h
	KVInfo.h
	au_map.h
)

SET (SOURCES
	LockDebugger.cpp
	Lock.cpp
	SamsonSetup.cpp
	traces.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")

ADD_LIBRARY(samsonCommonStatic STATIC ${SOURCES} ${HEADERS} ${HEADERS_SAMSON})
TARGET_LINK_LIBRARIES(samsonCommonStatic  protobuf logMsgStatic)
SET_TARGET_PROPERTIES(samsonCommonStatic PROPERTIES OUTPUT_NAME samsonCommon)

# ADD_LIBRARY(samsonCommonShared SHARED ${SOURCES} ${HEADERS} ${HEADERS_SAMSON})
# TARGET_LINK_LIBRARIES(samsonCommonShared  protobuf)
# SET_TARGET_PROPERTIES(samsonCommonShared PROPERTIES OUTPUT_NAME samsonCommon)



