# Name of the module being developed
SET( MODULE_NAME twitter )

cmake_minimum_required(VERSION 2.6)

ADD_CUSTOM_COMMAND(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Module.cpp
    COMMAND samsonModuleParser ${PROJECT_SOURCE_DIR}/modules/${MODULE_NAME}/module ${PROJECT_SOURCE_DIR}/modules/${MODULE_NAME} Module
)

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
include_directories("${CMAKE_INSTALL_PREFIX}/include")
include_directories("${SAMSON_HOME}/include")
include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/modules/${MODULE_NAME}")
include_directories("${PROJECT_SOURCE_DIR}/modules/system")
include_directories("${PROJECT_BINARY_DIR}/modules/${MODULE_NAME}")


MESSAGE( "----------------------------------------------------------------------------")
MESSAGE( "Creating module ${MODULE_NAME}" )
MESSAGE( "----------------------------------------------------------------------------")

ADD_LIBRARY(${MODULE_NAME} MODULE ${SOURCES} ${ADDITIONAL_SOURCES}  )
# Un-comment the following 2 lines to locate a shared/static library 
# that is not in the standard library path
# LD_LIBRARY_PATH (Linux) / DYLD_LIBRARY_PATH (Mac OS X)
# Where libname is the name of the library being added, e.g. mongoclient
# and PATH_TO_EXTERNAL_LIBRARY is the path (relative or absolute) to where
# libname can be found. If the library can be found its full path is assigned
# to the environment variable EXTERNAL_PATH.
#FIND_LIBRARY(EXTERNAL_PATH libname PATHS PATH_TO_EXTERNAL_LIBRARY )
#TARGET_LINK_LIBRARIES( ${MODULE_NAME} ${EXTERNAL_PATH} )

# Install location for the module, defaults to /opt/samson/module
INSTALL(TARGETS ${MODULE_NAME} DESTINATION modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/${MODULE_NAME})
