cmake_minimum_required(VERSION 2.6)

SET (SOURCES
    Module.h
    Module.cpp
)

SET (OPERATION_HEADERS
operations/fake_cdrs.h
operations/fake_users.h
operations/parse_cdrs.h
operations/parse_users.h
operations/remove_duplicated_cdrs.h
operations/spread_cdrs_to_links.h
operations/select_cdrs.h
)

# Install headers to be used by other modules

SET (HEADERS
samson/modules/cdr/CDR.h
samson/modules/cdr/CDR_base.h
samson/modules/cdr/Date.h
samson/modules/cdr/Date_base.h
samson/modules/cdr/Time.h
samson/modules/cdr/Time_base.h
samson/modules/cdr/User.h
samson/modules/cdr/User_base.h
)

#Generate 'Module.cpp' and 'Module.h' from ''Module''
add_custom_command (
  OUTPUT           ${CMAKE_CURRENT_SOURCE_DIR}/Module.h
  OUTPUT	   ${CMAKE_CURRENT_SOURCE_DIR}/Module.cpp
  COMMAND          samsonModuleParser ${CMAKE_CURRENT_SOURCE_DIR}/module
  MAIN_DEPENDENCY  module
)

include_directories("${PROJECT_SOURCE_DIR}",  "../../trunk/libs/logMsg")

ADD_LIBRARY(cdr MODULE ${SOURCES} ${HEADERS} ${OPERATION_HEADERS})

INSTALL(TARGETS cdr DESTINATION /opt/samson/modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/cdr)



