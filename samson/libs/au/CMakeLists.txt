
# ---------------------------------------------------
# mutex directory
# ---------------------------------------------------

SET( MUTEX_FILES
mutex/Lock.h
mutex/StopLock.h
mutex/LockDebugger.h
mutex/Token.h
mutex/TokenTaker.h
mutex/Lock.cpp
mutex/LockDebugger.cpp
mutex/Token.cpp
mutex/TokenTaker.cpp
)


# ---------------------------------------------------
# statistics directory
# ---------------------------------------------------

SET( STATISTICS_FILES
statistics/ActivityMonitor.h
statistics/ActivityMonitor.cpp
)

# ---------------------------------------------------
# log directory
# ---------------------------------------------------

SET( LOG_FILES
log/log_server_common.h

log/LogCentral.h
log/LogPlugin.h
log/Log.h
log/LogClient.h
log/LogServer.h
log/LogServerChannel.h
log/LogServerQuery.h
log/LogToServer.h
log/LogFile.h
log/LogFormatter.h
log/TableLogFormatter.h

log/LogCentral.cpp
log/LogPlugin.cpp
log/Log.cpp
log/LogClient.cpp
log/LogServer.cpp
log/LogServerChannel.cpp
log/LogServerQuery.cpp
log/LogToServer.cpp
log/LogFile.cpp
log/LogFormatter.cpp
log/TableLogFormatter.cpp
)

# ---------------------------------------------------
# containers directory
# ---------------------------------------------------

SET( CONTAINERS_FILES

containers/map.h
containers/simple_map.h
containers/list.h
containers/set.h
containers/vector.h
containers/ListMap.h

containers/StringVector.h
containers/Uint64Vector.h
containers/StringVector.cpp
containers/Uint64Vector.cpp
)

# ---------------------------------------------------
# tables directory
# ---------------------------------------------------

SET( TABLES_FILES
tables/Select.h
tables/Table.h
tables/Tree.h
tables/Collection.h
tables/DataBase.h
tables/pugi.h
tables/pugixml.hpp
tables/pugiconfig.hpp

tables/Select.cpp
tables/Table.cpp
tables/Tree.cpp
tables/Collection.cpp
tables/DataBase.cpp
tables/pugi.cpp
tables/pugixml.cpp
)

# ---------------------------------------------------
# network directory
# ---------------------------------------------------

SET( NETWORK_FILES
network/FileDescriptor.cpp
network/FileDescriptor.h
network/NetworkListener.cpp
network/NetworkListener.h
network/Service.cpp
network/Service.h
network/ConsoleService.h
network/ConsoleService.cpp
network/RESTService.h
network/RESTService.cpp
network/SocketConnection.cpp
network/SocketConnection.h
network/misc.cpp
network/misc.h
network/PacketReaderWriter.h
network/PacketReaderWriter.cpp
)

# ---------------------------------------------------
# console directory
# ---------------------------------------------------

SET( CONSOLE_FILES
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


# ---------------------------------------------------
# string directory
# ---------------------------------------------------

SET( STRING_FILES
string/split.h
string/split.cpp
)

# ---------------------------------------------------
# HEADERS & SOURCES
# ---------------------------------------------------

SET (HEADERS
Status.h
Object.h
Singleton.h
au.pb.h
TemporalBuffer.h
namespace.h
charset.h
utils.h
Descriptors.h
gpb.h
Environment.h
CommandLine.h

ErrorManager.h
varint.h
Cronometer.h
Rate.h
CounterCollection.h
file.h
time.h
string.h
xml.h
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

daemonize.h
)

SET (SOURCES
Status.cpp
Object.cpp
au.pb.cc
TemporalBuffer.cpp
charset.cpp
utils.cpp

Descriptors.cpp
gpb.cpp
CommandLine.cpp
Environment.cpp


ErrorManager.cpp
Cronometer.cpp
Rate.cpp
CounterCollection.cpp
file.cpp
time.cpp
string.cpp
xml.cpp
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

daemonize.cpp
)

# Source groups to organize Xcode project
# -----------------------------------------------------------------

SOURCE_GROUP( mutex      FILES ${MUTEX_FILES} )
SOURCE_GROUP( log        FILES ${LOG_FILES} )
SOURCE_GROUP( statistics FILES ${STATISTICS_FILES} )
SOURCE_GROUP( containers FILES ${CONTAINERS_FILES} )
SOURCE_GROUP( tables     FILES ${TABLES_FILES} )
SOURCE_GROUP( network    FILES ${NETWORK_FILES} )
SOURCE_GROUP( console    FILES ${CONSOLE_FILES} )
SOURCE_GROUP( string     FILES ${STRING_FILES} )

# Generate 'au.pb.h' and 'au.pb.cc' from ''au.proto''
add_custom_command (
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/au.pb.cc
  OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/au.pb.h
  COMMAND protoc ${CMAKE_CURRENT_SOURCE_DIR}/au.proto --cpp_out=${CMAKE_CURRENT_SOURCE_DIR} --proto_path=${CMAKE_CURRENT_SOURCE_DIR}
  MAIN_DEPENDENCY au.proto
)

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(au STATIC ${SOURCES} ${HEADERS} ${MUTEX_FILES} ${LOG_FILES} ${STATISTICS_FILES}
${CONTAINERS_FILES} ${TABLES_FILES} ${NETWORK_FILES} ${CONSOLE_FILES}
${STRING_FILES})

# Only needed for the "samson" project
IF (CMAKE_PROJECT_NAME STREQUAL "Samson")
    INSTALL(TARGETS au ARCHIVE DESTINATION lib)
    INSTALL(FILES ${HEADERS}          DESTINATION include/au)
    INSTALL(FILES ${STATISTICS_FILES} DESTINATION include/au/statistics)
    INSTALL(FILES ${MUTEX_FILES}      DESTINATION include/au/mutex)
    INSTALL(FILES ${LOG_FILES}        DESTINATION include/au/log)
    INSTALL(FILES ${CONTAINERS_FILES} DESTINATION include/au/containers)
    INSTALL(FILES ${TABLES_FILES}     DESTINATION include/au/tables)
    INSTALL(FILES ${NETWORK_FILES}    DESTINATION include/au/network)
    INSTALL(FILES ${CONSOLE_FILES}    DESTINATION include/au/console)
    INSTALL(FILES ${STRING_FILES}     DESTINATION include/au/string)
ENDIF (CMAKE_PROJECT_NAME STREQUAL "Samson")

SET (LIBAU ${PROJECT_BINARY_DIR}/libs/au/libau.a CACHE INTERNAL "")


# au_test ( for debuggin )
#ADD_EXECUTABLE(au_test au_test_main.cpp )
#TARGET_LINK_LIBRARIES(au_test ${LIBS} au lm )
#INSTALL(TARGETS               au_test        RUNTIME DESTINATION   bin)

# au_sleep_test
#ADD_EXECUTABLE(au_sleep_test au_sleep_test_main.cpp )
#TARGET_LINK_LIBRARIES(au_sleep_test ${LIBS} au lm pa )
#INSTALL(TARGETS au_sleep_test        RUNTIME DESTINATION   bin)









