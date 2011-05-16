cmake_minimum_required(VERSION 2.6)
SET (SOURCES
    Module.h
    Module.cpp
)

# Install headers to be used by other modules

SET (HEADERS
samson/modules/system/FixedLengthDataInstance.h
samson/modules/system/SimpleParser.h
samson/modules/system/SimpleParserOut.h
samson/modules/system/Double.h
samson/modules/system/Float.h
samson/modules/system/Int16.h
samson/modules/system/Int32.h
samson/modules/system/Int8.h
samson/modules/system/String.h
samson/modules/system/UInt.h
samson/modules/system/UInt16.h
samson/modules/system/UInt32.h
samson/modules/system/UInt64.h
samson/modules/system/UInt8.h
samson/modules/system/Void.h
samson/modules/system/UInt2.h
samson/modules/system/UInt2_base.h
samson/modules/system/Vector_UInt.h
samson/modules/system/Vector_UInt_base.h
samson/modules/system/Time.h
)

SET (OPERATION_HEADERS
operations/parse_id_id.h
operations/parser_uint_void.h
)

#Generate 'Module.cpp' and 'Module.h' from ''Module''
add_custom_command (
  OUTPUT           ${CMAKE_CURRENT_SOURCE_DIR}/Module.h
  OUTPUT	   ${CMAKE_CURRENT_SOURCE_DIR}/Module.cpp
  COMMAND          samsonModuleParser ${CMAKE_CURRENT_SOURCE_DIR}/module
  MAIN_DEPENDENCY  module
)

include_directories("${PROJECT_SOURCE_DIR}")

ADD_LIBRARY(system MODULE ${SOURCES} ${HEADERS} ${OPERATION_HEADERS})

INSTALL(TARGETS system DESTINATION /opt/samson/modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/system)


