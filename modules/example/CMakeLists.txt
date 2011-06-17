SET( MODULE_NAME example )

cmake_minimum_required(VERSION 2.6)

SET (SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/Module.cpp
)

# Take all .cpp file in the compilation
FILE( GLOB ADDITIONAL_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp  )

# Install headers to be used by other modules
FILE( GLOB HEADERS samson/modules/${MODULE_NAME}/*.h  )

# Include directory pointing to current dir and /usr/local/include
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("/usr/local/include/")

MESSAGE( "----------------------------------------------------------------------------")
MESSAGE( "Creating module ${MODULE_NAME}" )
MESSAGE( "----------------------------------------------------------------------------")

ADD_LIBRARY(${MODULE_NAME} MODULE ${SOURCES} ${ADDITIONAL_SOURCES}  )

INSTALL(TARGETS ${MODULE_NAME} DESTINATION /opt/samson/modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/${MODULE_NAME})


