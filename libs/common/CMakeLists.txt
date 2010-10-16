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
)

SET (SOURCES
	LockDebugger.cpp
	Lock.cpp
	SamsonSetup.cpp
)

ADD_LIBRARY(samsonCommonStatic STATIC ${SOURCES} ${HEADERS} ${HEADERES_SAMSON} )
TARGET_LINK_LIBRARIES( samsonCommonStatic  protobuf )
SET_TARGET_PROPERTIES(samsonCommonStatic PROPERTIES OUTPUT_NAME samsonCommon)

ADD_LIBRARY(samsonCommonShared SHARED ${SOURCES} ${HEADERS} ${HEADERES_SAMSON} )
TARGET_LINK_LIBRARIES( samsonCommonShared  protobuf )
SET_TARGET_PROPERTIES(samsonCommonShared PROPERTIES OUTPUT_NAME samsonCommon)



