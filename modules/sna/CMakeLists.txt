SET( MODULE_NAME sna )

cmake_minimum_required(VERSION 2.6)

SET (SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/Module.cpp
)

FILE( GLOB ADDITIONAL_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp  )


# Install headers to be used by other modules

FILE( GLOB HEADERS samson/modules/${MODULE_NAME}/*.h  )

SET (OPERATION_HEADERS
)

include_directories("${PROJECT_SOURCE_DIR}")

MESSAGE( "----------------------------------------------------------------------------")
MESSAGE( "Creating module ${MODULE_NAME}" )
MESSAGE( "----------------------------------------------------------------------------")
ADD_LIBRARY(${MODULE_NAME} MODULE ${SOURCES} ${ADDITIONAL_SOURCES}  )

INSTALL(TARGETS ${MODULE_NAME} DESTINATION /opt/samson/modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/${MODULE_NAME})



ENABLE_TESTING()
ADD_SUBDIRECTORY(testing)     # Test module and ctest stuff

