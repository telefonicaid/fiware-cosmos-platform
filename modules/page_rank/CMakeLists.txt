SET( MODULE_NAME page_rank )

cmake_minimum_required(VERSION 2.6)

# Install directory
SET (INSTALL_DIR $ENV{SAMSON_HOME})
if (INSTALL_DIR)
    MESSAGE("Using $ENV{SAMSON_HOME} as the installation location")
    SET (CMAKE_INSTALL_PREFIX $ENV{SAMSON_HOME} CACHE INTERNAL "")
else (NOT INSTALL_DIR)
    MESSAGE("Default to /opt/samson as the installation location. Set the environment variable SAMSON_HOME to override")
    SET (CMAKE_INSTALL_PREFIX /opt/samson CACHE INTERNAL "")
endif (INSTALL_DIR)

# Locate samsonModuleParser
if (SAMSONMODULEPARSER)
    MESSAGE ("Using ${SAMSONMODULEPARSER} to parse the module file")
else (NOT SAMSONMODULEPARSER)
    FIND_PROGRAM(HAVE_SAMSONMODULEPARSER samsonModuleParser HINTS ${PROJECT_BINARY_DIR}/apps/samsonModuleParser/samsonModuleParser ENV PATH)
    if (HAVE_SAMSONMODULEPARSER)
        SET (SAMSONMODULEPARSER ${HAVE_SAMSONMODULEPARSER} CACHE INTERNAL "")
    else (NOT HAVE_SAMSONMODULEPARSER)
        MESSAGE("*****************************")
        MESSAGE("")
        MESSAGE("samsonModuleParser not found")
        MESSAGE("")
        MESSAGE("*****************************")
    endif (HAVE_SAMSONMODULEPARSER)
endif (SAMSONMODULEPARSER)

IF (SVN_BUILD)
    ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Module.cpp
        COMMAND ${SAMSONMODULEPARSER} ${PROJECT_SOURCE_DIR}/modules/${MODULE_NAME}/module ${PROJECT_SOURCE_DIR}/modules/${MODULE_NAME} Module
    )
ELSE (NOT SVN_BUILD)
    ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Module.cpp
        COMMAND ${SAMSONMODULEPARSER} ${PROJECT_SOURCE_DIR}/module ${PROJECT_SOURCE_DIR} Module
    )
ENDIF (SVN_BUILD)

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

else (NOT CMAKE_BUILD_TYPE STREQUAL)

    MESSAGE("cmake: RELEASE compilation")
    add_definitions(-DRELEASE)
    set (CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas -D_LARGEFILE64_SOURCE -D_GNU_SOURCE -Werror -O2")

endif (CMAKE_BUILD_TYPE STREQUAL DEBUG)

# Include directory pointing to current dir and /usr/local/include
IF (SVN_BUILD)
    include_directories("${PROJECT_SOURCE_DIR}/libs")
    include_directories("${PROJECT_SOURCE_DIR}/modules/${MODULE_NAME}")
    include_directories("${PROJECT_SOURCE_DIR}/modules/system")
    include_directories("${PROJECT_BINARY_DIR}/modules/${MODULE_NAME}")
ELSE (NOT SVN_BUILD)
    include_directories("${PROJECT_SOURCE_DIR}")
    include_directories("${CMAKE_INSTALL_PREFIX}")
    include_directories("${CMAKE_INSTALL_PREFIX}/include/samson")
ENDIF (SVN_BUILD)

MESSAGE( "----------------------------------------------------------------------------")
MESSAGE( "Creating module ${MODULE_NAME}" )
MESSAGE( "----------------------------------------------------------------------------")

ADD_LIBRARY(${MODULE_NAME} MODULE ${SOURCES} ${ADDITIONAL_SOURCES}  )
if (NOT LIBLM)
    find_library(LM_PATH lm PATHS ${PROJECT_BINARY_DIR}/libs/logMsg)
    if (LM_PATH)
        SET (LIBLM ${LM_PATH})
    else (NOT LM_PATH)
        MESSAGE("Unable to find the static library liblm.a")
    endif (LM_PATH)
    TARGET_LINK_LIBRARIES( ${MODULE_NAME} ${LIBLM} )
else (LIBLM)
    TARGET_LINK_LIBRARIES( ${MODULE_NAME} lm )
endif (NOT LIBLM)

INSTALL(TARGETS ${MODULE_NAME} DESTINATION modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/${MODULE_NAME})



#ENABLE_TESTING()
#ADD_SUBDIRECTORY(testing)     # Test module and ctest stuff
