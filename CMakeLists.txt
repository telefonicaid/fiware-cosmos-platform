#
# Project name and basic prerequisities
#
PROJECT(Samson)
cmake_minimum_required(VERSION 2.6)

#
# DEBUG or RELEASE build ?
#

if (NOT CMAKE_BUILD_TYPE)
    MESSAGE("Fatal Error: please set CMAKE_BUILD_TYPE")
endif (NOT CMAKE_BUILD_TYPE)

set(error 0)

execute_process(COMMAND            echo $USER
                COMMAND            tr 'a-z' 'A-Z'
                OUTPUT_VARIABLE    USER)

add_definitions(-DLM_ON)
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

#
# Enabling testing in debug mode
#
if (CMAKE_BUILD_TYPE STREQUAL DEBUG)
   ENABLE_TESTING()
endif (CMAKE_BUILD_TYPE STREQUAL DEBUG)



# Package
# ----------------------------------------------------------------------

if (CMAKE_BUILD_TYPE STREQUAL RELEASE)

if (APPLE)
#   include(${PROJECT_SOURCE_DIR}/CPackConfigBUNDLE.cmake)
   include(${PROJECT_SOURCE_DIR}/CPackConfigTGZ.cmake)
else (APPLE)
   include(${PROJECT_SOURCE_DIR}/CPackConfigRPM.cmake)
endif (APPLE)

include(CPack)

endif (CMAKE_BUILD_TYPE STREQUAL RELEASE)

#
# Architecture 
#
set (CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Rogue")

#
# Common libraries
#

if(COVERAGE)
	SET( COMMON_SAMSON_LIBS gcov )
endif(COVERAGE)

#
# Continuing if OK
#
if (error EQUAL 0)
  MESSAGE("cmake: OK")
  ADD_SUBDIRECTORY(scripts)
  ADD_SUBDIRECTORY(libs)
  ADD_SUBDIRECTORY(apps)

if (CMAKE_BUILD_TYPE STREQUAL DEBUG)

#  ADD_SUBDIRECTORY(testing)	# Test module and ctest stuff

endif (CMAKE_BUILD_TYPE STREQUAL DEBUG)

#  ADD_SUBDIRECTORY(etc)	# Add setup file when installing

else ()
    MESSAGE("cmake: NOT OK")
endif (error EQUAL 0)


# Code coverage

if(COVERAGE)
  set_target_properties( 
		pa lm
		 samsonCore samsonCommon   samsonDataLib  samsonNetwork
		delilahLib samsonWorkerLib samsonControllerLib 
		test_memory test_disk test_hostMgr test_unit
                PROPERTIES
                COMPILE_FLAGS "-O0 --coverage -fprofile-arcs -ftest-coverage"
                LINK_FLAGS "--coverage -fprofile-arcs")

endif(COVERAGE)


