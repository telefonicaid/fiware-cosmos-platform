SET( MODULE_NAME tienda )

cmake_minimum_required(VERSION 2.6)

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
        COMMAND samsonModuleParser ${CMAKE_CURRENT_SOURCE_DIR}/module ${CMAKE_CURRENT_SOURCE_DIR} Module
        MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/module
	DEPENDS ${SAMSONMODULEPARSER}
    )
ELSE (NOT SVN_BUILD)
    ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/Module.cpp
        COMMAND ${SAMSONMODULEPARSER} ${PROJECT_SOURCE_DIR}/module ${PROJECT_SOURCE_DIR} Module
      	MAIN_DEPENDENCY ${PROJECT_SOURCE_DIR}/module
	DEPENDS ${SAMSONMODULEPARSER}
    )
ENDIF (SVN_BUILD)

SET (SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/Module.cpp
)

# Take all .cpp file in the compilation
FILE( GLOB ADDITIONAL_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp  )

# Install headers to be used by other modules
FILE( GLOB HEADERS samson/modules/${MODULE_NAME}/*.h  )

# Compiler options for DEBUG or RELEASE (copied from trunk/CMakeList)
add_definitions(-fPIC)

#Baseline compiler flags
set (CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas -D_LARGEFILE64_SOURCE -D_GNU_SOURCE -Werror")

if (CMAKE_BUILD_TYPE STREQUAL DEBUG)

    MESSAGE("cmake: DEBUG compilation")
    add_definitions(-DDEBUG -DDEBUG_$ENV{USER} -DDEBUG_${USER})
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

else (NOT CMAKE_BUILD_TYPE STREQUAL DEBUG)

    MESSAGE("cmake: RELEASE compilation")
    add_definitions(-DRELEASE)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

endif (CMAKE_BUILD_TYPE STREQUAL DEBUG)

# -Wunused-but-set-variable is available from gcc 4.6.0 but can be set in 4.5.x
# OS X comes with gcc 4.2.1 (10.7) and does not accept this flag
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-but-set-variable")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# Include directory pointing to current dir and /usr/local/include
IF (SVN_BUILD)
    include_directories("${PROJECT_SOURCE_DIR}/libs")
    include_directories("${CMAKE_CURRENT_SOURCE_DIR}")
    include_directories("${PROJECT_SOURCE_DIR}/modules/core/system")
    include_directories("${PROJECT_BINARY_DIR}/modules/${MODULE_NAME}")
ELSE (NOT SVN_BUILD)
    include_directories("${PROJECT_SOURCE_DIR}")
    include_directories("${CMAKE_INSTALL_PREFIX}")
    include_directories("${CMAKE_INSTALL_PREFIX}/include")
    include_directories("${CMAKE_INSTALL_PREFIX}/include/samson")
    link_directories("${CMAKE_INSTALL_PREFIX}/lib")
ENDIF (SVN_BUILD)


MESSAGE( "----------------------------------------------------------------------------")
MESSAGE( "Creating module ${MODULE_NAME}" )
MESSAGE( "----------------------------------------------------------------------------")

ADD_LIBRARY(${MODULE_NAME} MODULE ${SOURCES} ${ADDITIONAL_SOURCES}  )
TARGET_LINK_LIBRARIES( ${MODULE_NAME} lm )

INSTALL(TARGETS ${MODULE_NAME} DESTINATION modules)
INSTALL(FILES ${HEADERS} DESTINATION include/samson/modules/${MODULE_NAME})


#ENABLE_TESTING()
#ADD_SUBDIRECTORY(testing)     # Test module and ctest stuff
