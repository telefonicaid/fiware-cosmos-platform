SET(MODULE_NAME passive_location)

cmake_minimum_required(VERSION 2.6)

SET (SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/Module.cpp
)

# Take all .cpp file in the compilation
FILE(GLOB ADDITIONAL_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp)

# Install headers to be used by other modules
FILE( GLOB HEADERS samson/modules/${MODULE_NAME}/*.h  )

# Include directory pointing to current dir and /usr/local/include
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("/usr/local/include/")
include_directories("/opt/local/include/")
include_directories("/opt/mongodb/mongo-cxx-driver-v2.0")

link_directories("/opt/mongodb/mongo-cxx-driver-v2.0")
link_directories("/opt/local/lib")

MESSAGE( "----------------------------------------------------------------------------")
MESSAGE( "Creating module ${MODULE_NAME}" )
MESSAGE( "----------------------------------------------------------------------------")

find_package(Boost COMPONENTS system filesystem REQUIRED)

ADD_LIBRARY(${MODULE_NAME} MODULE ${SOURCES} ${ADDITIONAL_SOURCES}  )
target_link_libraries(${MODULE_NAME}  -lboost_thread-mt -lboost_filesystem-mt -lboost_system-mt -lmongoclient)

INSTALL(TARGETS ${MODULE_NAME} DESTINATION /opt/samson/modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/${MODULE_NAME})
