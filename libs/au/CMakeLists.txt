SET (HEADERS
status.h
Descriptors.h
gpb.h
Environment.h
CommandLine.h
Console.h
ErrorManager.h
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
Rate.h
Object.h
)

SET (SOURCES
Descriptors.cpp
CommandLine.cpp
Console.cpp
ErrorManager.cpp
Format.cpp
Lock.cpp
LockDebugger.cpp
Stopper.cpp
Token.cpp
Cronometer.cpp
Rate.cpp
Object.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")



# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(au STATIC ${SOURCES} ${HEADERS})



