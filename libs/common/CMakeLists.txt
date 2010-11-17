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
	ObjectWithStatus.h
)

SET (SOURCES
	LockDebugger.cpp
	Lock.cpp
	SamsonSetup.cpp
	traces.cpp
	ObjectWithStatus.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")

ADD_LIBRARY(samsonCommonStatic STATIC ${SOURCES} ${HEADERS} ${HEADERS_SAMSON})
TARGET_LINK_LIBRARIES(samsonCommonStatic  protobuf lm)

SET_TARGET_PROPERTIES(samsonCommonStatic PROPERTIES OUTPUT_NAME samsonCommon)

# ADD_LIBRARY(samsonCommonShared SHARED ${SOURCES} ${HEADERS} ${HEADERS_SAMSON})
# TARGET_LINK_LIBRARIES(samsonCommonShared  protobuf)
# SET_TARGET_PROPERTIES(samsonCommonShared PROPERTIES OUTPUT_NAME samsonCommon)



