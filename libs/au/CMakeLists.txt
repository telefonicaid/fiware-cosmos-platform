SET (HEADERS
Object.h
ObjectDictionary.h
ObjectVector.h
ObjectPool.h

namespace.h

utils.h
status.h
Descriptors.h
gpb.h
Environment.h
CommandLine.h

Console.h
ConsoleCommand.h
ConsoleCommandHistory.h
ConsoleEscapeSequence.h
ConsoleCode.h
ConsoleAutoComplete.h

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
CounterCollection.h
file.h
time.h
string.h
xml.h
Table.h
Tree.h
RRT.h
cpu.h
ProcessStats.h
)

SET (SOURCES

Object.cpp
ObjectDictionary.cpp
ObjectVector.cpp
ObjectPool.cpp

utils.cpp

Descriptors.cpp
CommandLine.cpp
Environment.cpp

Console.cpp
ConsoleCommand.cpp
ConsoleCommandHistory.cpp
ConsoleEscapeSequence.cpp
ConsoleCode.cpp

ConsoleAutoComplete.cpp
ErrorManager.cpp
Lock.cpp
LockDebugger.cpp
Token.cpp
TokenTaker.cpp
Cronometer.cpp
Rate.cpp
CounterCollection.cpp
file.cpp
time.cpp
string.cpp
xml.cpp
Table.cpp
Tree.cpp
RRT.cpp
cpu.cpp
ProcessStats.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")



# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(au STATIC ${SOURCES} ${HEADERS})
INSTALL(TARGETS au ARCHIVE DESTINATION lib)
SET (LIBAU ${PROJECT_BINARY_DIR}/libs/au/libau.a CACHE INTERNAL "")


# au_test ( for debuggin )
#ADD_EXECUTABLE(au_test au_test_main.cpp )
#TARGET_LINK_LIBRARIES(au_test ${LIBS} au lm )
#INSTALL(TARGETS               au_test        RUNTIME DESTINATION   bin)

# au_sleep_test
ADD_EXECUTABLE(au_sleep_test au_sleep_test_main.cpp )
TARGET_LINK_LIBRARIES(au_sleep_test ${LIBS} au lm pa )
INSTALL(TARGETS au_sleep_test        RUNTIME DESTINATION   bin)

# Unit testing
ADD_EXECUTABLE( ut_au_object ut_au_object.cpp )
TARGET_LINK_LIBRARIES( ut_au_object ${LIBS} au lm pa )
INSTALL(TARGETS  ut_au_object RUNTIME DESTINATION   bin)


