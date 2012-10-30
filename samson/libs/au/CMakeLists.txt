
# ---------------------------------------------------
# setup directory
# ---------------------------------------------------

SET( SETUP_FILES
setup/SetupItem.h
setup/SetupItemCollection.h
setup/SetupItem.cpp
setup/SetupItemCollection.cpp
)

# ---------------------------------------------------
# zoo directory
# ---------------------------------------------------

SET( ZOO_FILES
zoo/Connection.cpp
zoo/Connection.h
zoo/ZooNodeCommiter.cpp
zoo/ZooNodeCommiter.h
zoo/common.cpp
zoo/common.h
)

# ---------------------------------------------------
# singleton directory
# ---------------------------------------------------

SET( SINGLETON_FILES
singleton/Singleton.h
singleton/SingletonManager.h
singleton/Singleton.cpp
singleton/SingletonManager.cpp
)

# ---------------------------------------------------
# mutex directory
# ---------------------------------------------------

SET( MUTEX_FILES
mutex/LockDebugger.h
mutex/Token.h
mutex/TokenTaker.h
mutex/LockDebugger.cpp
mutex/Token.cpp
mutex/TokenTaker.cpp
)




# ---------------------------------------------------
# statistics directory
# ---------------------------------------------------

SET( STATISTICS_FILES
statistics/ActivityMonitor.cpp
statistics/ActivityMonitor.h
statistics/CounterCollection.cpp
statistics/CounterCollection.h
statistics/Cronometer.cpp
statistics/Cronometer.h
statistics/CronometerSystem.cpp
statistics/CronometerSystem.h
statistics/OnOffMonitor.cpp
statistics/OnOffMonitor.h
statistics/ProcessStats.cpp
statistics/ProcessStats.h
statistics/Rate.cpp
statistics/Rate.h
)

# ---------------------------------------------------
# log directory
# ---------------------------------------------------

SET( LOG_FILES
log/LogMain.h
log/LogCommon.h
log/Log.h
log/LogCounter.h
log/LogChannelFilter.h
log/LogChannels.h
log/LogCentral.h
log/LogPlugin.h
log/LogPluginServer.h
log/LogPluginScreen.h
log/LogPluginConsole.h
log/LogPluginFile.h
log/LogServer.h
log/LogServerChannel.h
log/LogFile.h
log/LogFormatter.h
log/TableLogFormatter.h
log/LogContainer.h

log/Log.cpp
log/LogCounter.cpp
log/LogChannelFilter.cpp
log/LogChannels.cpp
log/LogCentral.cpp
log/LogPlugin.cpp
log/LogPluginServer.cpp
log/LogPluginScreen.cpp
log/LogPluginConsole.cpp
log/LogPluginFile.cpp
log/LogServer.cpp
log/LogServerChannel.cpp
log/LogFile.cpp
log/LogFormatter.cpp
log/TableLogFormatter.cpp
log/LogContainer.cpp
)

# ---------------------------------------------------
# containers directory
# ---------------------------------------------------

SET( CONTAINERS_FILES

containers/SharedPointer.h
containers/Dictionary.h
containers/GeneralDictionary.h
containers/Queue.h
containers/Box.h
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
network/RESTServiceCommand.h
network/RESTServiceCommand.cpp
network/SocketConnection.cpp
network/SocketConnection.h
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
console/ConsoleAutoComplete.h
console/CommandCatalogue.h

console/Console.cpp
console/ConsoleEntry.cpp
console/ConsoleCommand.cpp
console/ConsoleCommandHistory.cpp
console/ConsoleEscapeSequence.cpp
console/ConsoleAutoComplete.cpp
console/CommandCatalogue.cpp
)


# ---------------------------------------------------
# string directory
# ---------------------------------------------------

SET( STRING_FILES
string/Descriptors.cpp
string/Descriptors.h
string/Pattern.cpp
string/Pattern.h
string/S.h
string/StringCollection.cpp
string/StringCollection.h
string/StringComponents.cpp
string/StringComponents.h
string/Tokenizer.cpp
string/Tokenizer.h
string/StringUtilities.h
string/StringUtilities.cpp
string/xml.cpp
string/xml.h
)

# ---------------------------------------------------
# HEADERS & SOURCES
# ---------------------------------------------------

SET (HEADERS
Status.h
Log.h
Log.cpp
au.pb.h
TemporalBuffer.h
utils.h
gpb.h
Environment.h
CommandLine.h
ErrorManager.h
varint.h
ExecesiveTimeAlarm.h
file.h
time.h
cpu.h
ThreadManager.h
StructCollection.h
Pool.h
daemonize.h
)

SET (SOURCES
Status.cpp
au.pb.cc
TemporalBuffer.cpp
utils.cpp
gpb.cpp
CommandLine.cpp
Environment.cpp
ErrorManager.cpp
ExecesiveTimeAlarm.cpp
file.cpp
time.cpp
cpu.cpp
ThreadManager.cpp
StructCollection.cpp
Pool.cpp
daemonize.cpp
)

# Source groups to organize Xcode project
# -----------------------------------------------------------------

SOURCE_GROUP( mutex         FILES ${MUTEX_FILES} )
SOURCE_GROUP( singleton     FILES ${SINGLETON_FILES} )
SOURCE_GROUP( zoo           FILES ${ZOO_FILES} )
SOURCE_GROUP( setup         FILES ${SETUP_FILES} )
SOURCE_GROUP( log           FILES ${LOG_FILES} )
SOURCE_GROUP( statistics    FILES ${STATISTICS_FILES} )
SOURCE_GROUP( containers    FILES ${CONTAINERS_FILES} )
SOURCE_GROUP( tables        FILES ${TABLES_FILES} )
SOURCE_GROUP( network       FILES ${NETWORK_FILES} )
SOURCE_GROUP( console       FILES ${CONSOLE_FILES} )
SOURCE_GROUP( string        FILES ${STRING_FILES} )

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
ADD_LIBRARY(au STATIC ${SOURCES} ${HEADERS} ${MUTEX_FILES} ${SINGLETON_FILES} ${ZOO_FILES} ${SETUP_FILES}  ${LOG_FILES} ${STATISTICS_FILES}
${CONTAINERS_FILES} ${TABLES_FILES} ${NETWORK_FILES} ${CONSOLE_FILES}
${STRING_FILES})

# Only needed for the "samson" project
IF (CMAKE_PROJECT_NAME STREQUAL "Samson")
    INSTALL(TARGETS au ARCHIVE DESTINATION lib)
    INSTALL(FILES ${HEADERS}              DESTINATION include/au)
    INSTALL(FILES ${STATISTICS_FILES}     DESTINATION include/au/statistics)
    INSTALL(FILES ${MUTEX_FILES}          DESTINATION include/au/mutex)
    INSTALL(FILES ${SINGLETON_FILES}      DESTINATION include/au/singleton)
    INSTALL(FILES ${ZOO_FILES}            DESTINATION include/au/zoo)
    INSTALL(FILES ${SETUP_FILES}          DESTINATION include/au/setup)
    INSTALL(FILES ${LOG_FILES}            DESTINATION include/au/log)
    INSTALL(FILES ${CONTAINERS_FILES}     DESTINATION include/au/containers)
    INSTALL(FILES ${TABLES_FILES}         DESTINATION include/au/tables)
    INSTALL(FILES ${NETWORK_FILES}        DESTINATION include/au/network)
    INSTALL(FILES ${CONSOLE_FILES}        DESTINATION include/au/console)
    INSTALL(FILES ${STRING_FILES}         DESTINATION include/au/string)
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









