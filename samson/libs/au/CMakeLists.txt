
# ---------------------------------------------------
# HEADERS & SOURCES
# ---------------------------------------------------

  FILE( GLOB headers_au_console    "console/*.h" )
  FILE( GLOB headers_au_containers "containers/*.h" )
  FILE( GLOB headers_au_log        "log/*.h" )
  FILE( GLOB headers_au_mutex      "mutex/*.h" )
  FILE( GLOB headers_au_network    "network/*.h" )
  FILE( GLOB headers_au_setup      "setup/*.h" )
  FILE( GLOB headers_au_singleton  "singleton/*.h" )
  FILE( GLOB headers_au_statistics "statistics/*.h" )
  FILE( GLOB headers_au_string     "string/*.h" )
  FILE( GLOB headers_au_tables     "tables/*.h" )

  FILE( GLOB source_au_console    "console/*.cpp" )
  FILE( GLOB source_au_containers "containers/*.cpp" )
  FILE( GLOB source_au_log        "log/*.cpp" )
  FILE( GLOB source_au_mutex      "mutex/*.cpp" )
  FILE( GLOB source_au_network    "network/*.cpp" )
  FILE( GLOB source_au_setup      "setup/*.cpp" )
  FILE( GLOB source_au_singleton  "singleton/*.cpp" )
  FILE( GLOB source_au_statistics "statistics/*.cpp" )
  FILE( GLOB source_au_string     "string/*.cpp" )
  FILE( GLOB source_au_tables     "tables/*.cpp" )


SET (HEADERS
au.pb.h
CommandLine.h
console/CommandCatalogue.h
console/Console.h
console/ConsoleAutoComplete.h
console/ConsoleCommand.h
console/ConsoleCommandHistory.h
console/ConsoleEntry.h
console/ConsoleEscapeSequence.h
containers/Box.h
containers/Dictionary.h
containers/GeneralDictionary.h
containers/list.h
containers/ListMap.h
containers/map.h
containers/Queue.h
containers/set.h
containers/SharedPointer.h
containers/simple_map.h
containers/StringVector.h
containers/Uint64Vector.h
containers/vector.h
cpu.h
daemonize.h
Environment.h
ErrorManager.h
ExecesiveTimeAlarm.h
file.h
gpb.h
log/Log.h
log/LogQuery.h
log/LogFilter.h
log/LogProbe.h
log/LogCentral.h
log/LogCentralChannels.h
log/LogCentralPlugin.h
log/LogCentralChannelsFilter.h
log/LogCentralPluginConsole.h
log/LogCentralPluginFile.h
log/LogCentralPluginScreen.h
log/LogCentralPluginServer.h
log/LogCommon.h
log/LogContainer.h
log/LogCounter.h
log/LogFormatter.h
log/LogMain.h
log/LogServer.h
log/LogServerService.h
Log.h
mutex/LockDebugger.h
mutex/Token.h
mutex/TokenTaker.h
network/ConsoleService.h
network/FileDescriptor.h
network/NetworkListener.h
network/RESTService.h
network/RESTServiceCommand.h
network/Service.h
network/SocketConnection.h
Pool.h
setup/SetupItem.h
setup/SetupItemCollection.h
singleton/Singleton.h
singleton/SingletonManager.h
statistics/ActivityMonitor.h
statistics/CounterCollection.h
statistics/Cronometer.h
statistics/CronometerSystem.h
statistics/OnOffMonitor.h
statistics/ProcessStats.h
statistics/Rate.h
Status.h
string/Descriptors.h
string/Pattern.h
string/S.h
string/StringCollection.h
string/StringComponents.h
string/StringUtilities.h
string/Tokenizer.h
string/xml.h
StructCollection.h
tables/Collection.h
tables/DataBase.h
tables/pugi.h
tables/Select.h
tables/Table.h
tables/Tree.h
TemporalBuffer.h
ThreadManager.h
time.h
utils.h
varint.h
)

SET (SOURCES
au.pb.cc 
CommandLine.cpp
console/CommandCatalogue.cpp
console/Console.cpp
console/ConsoleAutoComplete.cpp
console/ConsoleCommand.cpp
console/ConsoleCommandHistory.cpp
console/ConsoleEntry.cpp
console/ConsoleEscapeSequence.cpp
containers/StringVector.cpp
containers/Uint64Vector.cpp
cpu.cpp
daemonize.cpp
Environment.cpp
ErrorManager.cpp
ExecesiveTimeAlarm.cpp
file.cpp
gpb.cpp
log/Log.cpp
log/LogQuery.cpp
log/LogFilter.cpp
log/LogProbe.cpp
log/LogCentral.cpp
log/LogCentralChannels.cpp
log/LogCentralPlugin.cpp
log/LogCentralChannelsFilter.cpp
log/LogCentralPluginConsole.cpp
log/LogCentralPluginFile.cpp
log/LogCentralPluginScreen.cpp
log/LogCentralPluginServer.cpp
log/LogContainer.cpp
log/LogCounter.cpp
log/LogFormatter.cpp
log/LogServer.cpp
log/LogServerService.cpp
Log.cpp
mutex/LockDebugger.cpp
mutex/Token.cpp
mutex/TokenTaker.cpp
network/ConsoleService.cpp
network/FileDescriptor.cpp
network/NetworkListener.cpp
network/RESTService.cpp
network/RESTServiceCommand.cpp
network/Service.cpp
network/SocketConnection.cpp
Pool.cpp
setup/SetupItem.cpp
setup/SetupItemCollection.cpp
singleton/Singleton.cpp
singleton/SingletonManager.cpp
statistics/ActivityMonitor.cpp
statistics/CounterCollection.cpp
statistics/Cronometer.cpp
statistics/CronometerSystem.cpp
statistics/OnOffMonitor.cpp
statistics/ProcessStats.cpp
statistics/Rate.cpp
Status.cpp
string/Descriptors.cpp
string/Pattern.cpp
string/StringCollection.cpp
string/StringComponents.cpp
string/StringUtilities.cpp
string/Tokenizer.cpp
string/xml.cpp
StructCollection.cpp
tables/Collection.cpp
tables/DataBase.cpp
tables/pugi.cpp
tables/pugixml.cpp
tables/Select.cpp
tables/Table.cpp
tables/Tree.cpp
TemporalBuffer.cpp
ThreadManager.cpp
time.cpp
utils.cpp
)

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
ADD_LIBRARY(au SHARED ${SOURCES} ${HEADERS} )
ADD_DEPENDENCIES(au au.pb.h)
ADD_DEPENDENCIES(au au.pb.cc)
TARGET_LINK_LIBRARIES( au protobuf pa lm )

# Only needed for the "samson" project
IF (CMAKE_PROJECT_NAME STREQUAL "Samson")
  INSTALL(TARGETS au DESTINATION lib)

  INSTALL(FILES ${headers_au_console}    DESTINATION include/au/console )
  INSTALL(FILES ${headers_au_containers} DESTINATION include/au/containers )
  INSTALL(FILES ${headers_au_log}        DESTINATION include/au/log )
  INSTALL(FILES ${headers_au_mutex}      DESTINATION include/au/mutex )
  INSTALL(FILES ${headers_au_network}    DESTINATION include/au/network )
  INSTALL(FILES ${headers_au_setup}      DESTINATION include/au/setup )
  INSTALL(FILES ${headers_au_singleton}  DESTINATION include/au/singleton )
  INSTALL(FILES ${headers_au_statistics} DESTINATION include/au/statistics )
  INSTALL(FILES ${headers_au_string}     DESTINATION include/au/string )
  INSTALL(FILES ${headers_au_tables}     DESTINATION include/au/tables )

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

# -----------------------------------------------------------------
# Source groups to organize Xcode project
# -----------------------------------------------------------------

SOURCE_GROUP( mutex       FILES ${headers_au_mutex} )
SOURCE_GROUP( singleton   FILES ${headers_au_singleton} )
SOURCE_GROUP( zoo         FILES ${headers_au_zoo} )
SOURCE_GROUP( setup       FILES ${headers_au_setup} )
SOURCE_GROUP( log         FILES ${headers_au_log} )
SOURCE_GROUP( statistics  FILES ${headers_au_statistics} )
SOURCE_GROUP( containers  FILES ${headers_au_containers} )
SOURCE_GROUP( tables      FILES ${headers_au_tables} )
SOURCE_GROUP( network     FILES ${headers_au_network} )
SOURCE_GROUP( console     FILES ${headers_au_console} )
SOURCE_GROUP( string      FILES ${headers_au_string} )









