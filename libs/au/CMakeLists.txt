SET (HEADERS
status.h
Descriptors.h
gpb.h
Environment.h
CommandLine.h
Console.h
ErrorManager.h
Lock.h
LockDebugger.h
StopLock.h
Token.h
TokenTaker.h
map.h
simple_map.h
list.h
set.h
ListMap.h
varint.h
Cronometer.h
Rate.h
CursesConsole.h
CounterCollection.h
file.h
time.h
string.h
xml.h
Table.h
Tree.h
RRT.h
)

SET (SOURCES
Descriptors.cpp
CommandLine.cpp
Environment.cpp
Console.cpp
ErrorManager.cpp
Lock.cpp
LockDebugger.cpp
Token.cpp
TokenTaker.cpp
Cronometer.cpp
Rate.cpp
CursesConsole.cpp
CounterCollection.cpp
file.cpp
time.cpp
string.cpp
xml.cpp
Table.cpp
Tree.cpp
RRT.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")



# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(au STATIC ${SOURCES} ${HEADERS})
INSTALL(TARGETS au ARCHIVE DESTINATION lib)
