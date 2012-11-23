
# ---------------------------------------------------
# HEADERS & SOURCES
# ---------------------------------------------------

  FILE( GLOB headers_au            "*.h" )
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

  FILE( GLOB source_au            "*.cpp" )
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


SET (EXTRA_HEADERS
au.pb.h
)

SET (EXTRA_SOURCES
au.pb.cc 
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
ADD_LIBRARY(au SHARED ${EXTRA_SOURCES} ${source_au} ${source_au_console} ${source_au_containers} ${source_au_log} ${source_au_log} ${source_au_mutex} ${source_au_network} ${source_au_network} ${source_au_network} ${source_au_setup} ${source_au_singleton} ${source_au_statistics} ${source_au_string} ${source_au_tables} ${headers_au} ${headers_au_console} ${headers_au_containers} ${headers_au_log} ${headers_au_mutex} ${headers_au_network} ${headers_au_setup} ${headers_au_singleton} ${headers_au_statistics} ${headers_au_statistics} ${headers_au_string} ${headers_au_tables} )
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

SOURCE_GROUP( au          FILES ${headers_au} )
SOURCE_GROUP( extra       FILES ${EXTRA_HEADERS} )
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









