SET (HEADERS
gpb.h
Environment.h
CommandLine.h
Console.h
Error.h
Format.h
Lock.h
LockDebugger.h
StopLock.h
Stopper.h
Token.h
map.h
list.h
set.h
ListMap.h
varint.h
Cronometer.h
)

SET (SOURCES
CommandLine.cpp
Console.cpp
Error.cpp
Format.cpp
Lock.cpp
LockDebugger.cpp
Stopper.cpp
Token.cpp
Cronometer.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(au STATIC ${SOURCES} ${HEADERS})



