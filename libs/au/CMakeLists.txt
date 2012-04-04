SET (HEADERS
Status.h

Object.h
namespace.h
charset.h
utils.h
status.h
Descriptors.h
gpb.h
Environment.h
CommandLine.h

Console.h
ConsoleEntry.h
ConsoleCommand.h
ConsoleCommandHistory.h
ConsoleEscapeSequence.h
ConsoleCode.h
ConsoleAutoComplete.h

network/misc.h
network/FileDescriptor.h
network/NetworkListener.h
network/SocketConnection.h

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
vector.h
ListMap.h
varint.h
Cronometer.h
Rate.h
CounterCollection.h
file.h
time.h
string.h
StringVector.h
xml.h
Table.h
RRT.h
cpu.h
ProcessStats.h

ThreadManager.h

StringCollection.h
StructCollection.h

Pool.h
StringComponents.h

Tokenizer.h

OnOffMonitor.h
)

SET (SOURCES
Status.cpp

Object.cpp
charset.cpp
utils.cpp

Descriptors.cpp
CommandLine.cpp
Environment.cpp

Console.cpp
ConsoleEntry.cpp
ConsoleCommand.cpp
ConsoleCommandHistory.cpp
ConsoleEscapeSequence.cpp
ConsoleCode.cpp

network/misc.cpp
network/FileDescriptor.cpp
network/NetworkListener.cpp
network/SocketConnection.cpp

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
StringVector.cpp
xml.cpp
Table.cpp
RRT.cpp
cpu.cpp
ProcessStats.cpp

ThreadManager.cpp

StringCollection.cpp
StructCollection.cpp

Pool.cpp
StringComponents.cpp

Tokenizer.cpp

OnOffMonitor.cpp
)

# Source groups to organize Xcode project
# -----------------------------------------------------------------

SOURCE_GROUP( network FILES
   network/misc.h
   network/FileDescriptor.h
   network/NetworkListener.h
   network/SocketConnection.h 

   network/misc.cpp
   network/FileDescriptor.cpp
   network/NetworkListener.cpp
   network/SocketConnection.cpp 
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(au STATIC ${SOURCES} ${HEADERS})
INSTALL(TARGETS au ARCHIVE DESTINATION lib)
INSTALL(FILES ${HEADERS} DESTINATION include/au)
SET (LIBAU ${PROJECT_BINARY_DIR}/libs/au/libau.a CACHE INTERNAL "")


# au_test ( for debuggin )
#ADD_EXECUTABLE(au_test au_test_main.cpp )
#TARGET_LINK_LIBRARIES(au_test ${LIBS} au lm )
#INSTALL(TARGETS               au_test        RUNTIME DESTINATION   bin)

# au_sleep_test
#ADD_EXECUTABLE(au_sleep_test au_sleep_test_main.cpp )
#TARGET_LINK_LIBRARIES(au_sleep_test ${LIBS} au lm pa )
#INSTALL(TARGETS au_sleep_test        RUNTIME DESTINATION   bin)








