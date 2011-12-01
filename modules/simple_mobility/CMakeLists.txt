SET( MODULE_NAME simple_mobility )

cmake_minimum_required(VERSION 2.6)

SET (SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/Module.cpp
)

# Take all .cpp file in the compilation
FILE( GLOB ADDITIONAL_SOURCES ${PROJECT_SOURCE_DIR}/*.cpp  )

# Install headers to be used by other modules
FILE( GLOB HEADERS samson/modules/${MODULE_NAME}/*.h  )

# Compiler options for DEBUG or RELEASE (copied from trunk/CMakeList)
add_definitions(-fPIC)
if (CMAKE_BUILD_TYPE STREQUAL DEBUG)

  MESSAGE("cmake: DEBUG compilation")
  add_definitions(-DDEBUG -DDEBUG_$ENV{USER} -DDEBUG_${USER})
  set (CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas -D_LARGEFILE64_SOURCE -D_GNU_SOURCE -g -Werror")

elseif (CMAKE_BUILD_TYPE STREQUAL RELEASE)

    MESSAGE("cmake: RELEASE compilation")
    add_definitions(-DRELEASE)
    set (CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas -D_LARGEFILE64_SOURCE -D_GNU_SOURCE -Werror -O2")

else (CMAKE_BUILD_TYPE STREQUAL DEBUG)
    MESSAGE("cmake: bad value for CMAKE_BUILD_TYPE - please set the variable to either DEBUG or RELEASE")
    set(error 1)
endif (CMAKE_BUILD_TYPE STREQUAL DEBUG)

# Include directory pointing to current dir and /usr/local/include
include_directories("${PROJECT_SOURCE_DIR}")
include_directories("/usr/local/include/")
include_directories("${PROJECT_SOURCE_DIR}/../../libs")
include_directories("${CMAKE_INSTALL_PREFIX}/include")

MESSAGE( "----------------------------------------------------------------------------")
MESSAGE( "Creating module ${MODULE_NAME}" )
MESSAGE( "----------------------------------------------------------------------------")

ADD_LIBRARY(${MODULE_NAME} MODULE ${SOURCES} ${ADDITIONAL_SOURCES}  )
if (CMAKE_BUILD_TYPE STREQUAL DEBUG)
    find_library(LM_PATH lm PATHS ${PROJECT_SOURCE_DIR}/../../BUILD_DEBUG/libs/logMsg )
    find_library(AU_PATH au PATHS ${PROJECT_SOURCE_DIR}/../../BUILD_DEBUG/libs/au )
elseif (CMAKE_BUILD_TYPE STREQUAL RELEASE)
    find_library(LM_PATH lm PATHS ${PROJECT_SOURCE_DIR}/../../BUILD_RELEASE/libs/logMsg )
    find_library(AU_PATH au PATHS ${PROJECT_SOURCE_DIR}/../../BUILD_RELEASE/libs/au )
endif (CMAKE_BUILD_TYPE STREQUAL DEBUG)
TARGET_LINK_LIBRARIES( ${MODULE_NAME} ${LM_PATH} ${AU_PATH} )

INSTALL(TARGETS ${MODULE_NAME} DESTINATION modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/${MODULE_NAME})

#ENABLE_TESTING()
#ADD_SUBDIRECTORY(testing)     # Test module and ctest stuff
