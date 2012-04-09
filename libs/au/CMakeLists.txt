SET (HEADERS
Status.h

Object.h
namespace.h
charset.h
utils.h
Descriptors.h
gpb.h
Environment.h
CommandLine.h

containers/map.h
containers/simple_map.h
containers/list.h
containers/set.h
containers/vector.h
containers/ListMap.h

console/Console.h
console/ConsoleEntry.h
console/ConsoleCommand.h
console/ConsoleCommandHistory.h
console/ConsoleEscapeSequence.h
console/ConsoleCode.h
console/ConsoleAutoComplete.h

string/split.h

network/misc.h
network/FileDescriptor.h
network/NetworkListener.h
network/SocketConnection.h
network/Service.h

tables/Select.h
tables/Table.h
tables/Tree.h
tables/Collection.h
tables/DataBase.h
tables/pugi.h
tables/pugixml.hpp
tables/pugiconfig.hpp

log/Log.h
log/LogServer.h
log/LogServerChannel.h
log/LogServerQuery.h
log/LogToServer.h
log/log_server_common.h

ErrorManager.h
Lock.h
LockDebugger.h
StopLock.h
Token.h
TokenTaker.h
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

console/Console.cpp
console/ConsoleEntry.cpp
console/ConsoleCommand.cpp
console/ConsoleCommandHistory.cpp
console/ConsoleEscapeSequence.cpp
console/ConsoleCode.cpp
console/ConsoleAutoComplete.cpp

string/split.cpp

network/misc.cpp
network/FileDescriptor.cpp
network/NetworkListener.cpp
network/SocketConnection.cpp
network/Service.cpp

tables/Select.cpp
tables/Table.cpp
tables/Tree.cpp
tables/Collection.cpp
tables/DataBase.cpp
tables/pugi.cpp
tables/pugixml.cpp

log/Log.cpp
log/LogServerChannel.cpp
log/LogServerQuery.cpp
log/LogToServer.cpp
log/Logserver.cpp

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

SOURCE_GROUP( containers FILES
containers/map.h
containers/simple_map.h
containers/list.h
containers/set.h
containers/vector.h
containers/ListMap.h
)

SOURCE_GROUP( console FILES
   console/Console.h
   console/ConsoleEntry.h
   console/ConsoleCommand.h
   console/ConsoleCommandHistory.h
   console/ConsoleEscapeSequence.h
   console/ConsoleCode.h
   console/ConsoleAutoComplete.h
   console/Console.cpp
   console/ConsoleEntry.cpp
   console/ConsoleCommand.cpp
   console/ConsoleCommandHistory.cpp
   console/ConsoleEscapeSequence.cpp
   console/ConsoleCode.cpp
   console/ConsoleAutoComplete.cpp
)


SOURCE_GROUP( string FILES
   string/split.h 
   string/split.cpp
)

SOURCE_GROUP( log FILES
   log/Log.h
   log/LogServer.h
   log/LogServerChannel.h
   log/LogServerQuery.h
   log/LogToServer.h
   log/log_server_common.h
   log/Log.cpp
   log/LogServer.cpp
   log/LogServerChannel.cpp
   log/LogServerQuery.cpp
   log/LogToServer.cpp
   log/log_server_common.cpp
)

SOURCE_GROUP( network FILES
   network/misc.h
   network/FileDescriptor.h
   network/NetworkListener.h
   network/SocketConnection.h 
   network/Service.h

   network/misc.cpp
   network/FileDescriptor.cpp
   network/NetworkListener.cpp
   network/SocketConnection.cpp 
   network/Servie.cpp
)


SOURCE_GROUP( tables FILES
	tables/Select.cpp
	tables/Table.cpp
	tables/Tree.cpp
	tables/Collection.cpp
	tables/DataBase.cpp
	tables/pugi.cpp
	tables/pugixml.cpp
	tables/Select.h
	tables/Table.h
	tables/Tree.h
	tables/Collection.h
	tables/DataBase.h
	tables/pugi.h
	tables/pugixml.h
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









