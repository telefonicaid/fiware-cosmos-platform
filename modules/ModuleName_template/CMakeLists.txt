SET( MODULE_NAME ModuleName_template )

cmake_minimum_required(VERSION 2.6)

SET (SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/Module.cpp
)

# Install headers to be used by other modules

FILE( GLOB HEADERS samson/modules/${MODULE_NAME}/*.h  )

SET (OPERATION_HEADERS
operations/parse_id_id.h
operations/parser_uint_void.h
)

include_directories("${PROJECT_SOURCE_DIR}")

MESSAGE( "----------------------------------------------------------------------------")
MESSAGE( "Creating module ${MODULE_NAME}" )
MESSAGE( "----------------------------------------------------------------------------")
ADD_LIBRARY(${MODULE_NAME} MODULE ${SOURCES}   )

INSTALL(TARGETS ${MODULE_NAME} DESTINATION /opt/samson/modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/${MODULE_NAME})



ENABLE_TESTING()
ADD_SUBDIRECTORY(testing)     # Test module and ctest stuff

