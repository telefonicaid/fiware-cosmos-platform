SET (HEADERS
	samsonLogMsg.h
	CommandLine.h
	Console.h
	Format.h
	LockDebugger.h
	Lock.h
	StopLock.h
)

SET (SOURCES
	LockDebugger.cpp
	Lock.cpp
)

ADD_LIBRARY(samsonCommonStatic STATIC ${SOURCES} ${HEADERS} )
ADD_LIBRARY(samsonCommonShared SHARED ${SOURCES} ${HEADERS} )

TARGET_LINK_LIBRARIES( samsonCommonStatic  protobuf )
TARGET_LINK_LIBRARIES( samsonCommonShared  protobuf )

SET_TARGET_PROPERTIES(samsonCommonShared PROPERTIES OUTPUT_NAME samsonCommon)
SET_TARGET_PROPERTIES(samsonCommonStatic PROPERTIES OUTPUT_NAME samsonCommon)



