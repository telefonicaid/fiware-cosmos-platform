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


#
# Common include
#
include_directories("/usr/include")
include_directories("/usr/local/include")
include_directories("/opt/local/include")

#
# Library directories
#
link_directories("/opt/local/lib")
link_directories("/usr/lib64")

#
# External dependency checks
#
# See if we have KDChart installed (check_include_files does not work due to these being C++ headers)
FIND_PATH(HAVE_KDCHART KDChartChart PATHS /usr/local/KDAB/KDChart-2.3.0/include /usr/local/kdchart/include /usr/local/KDAB/KDChart-2.4.2/include)
if (HAVE_KDCHART)
  MESSAGE("Using ${HAVE_KDCHART} for KDChart")
  include_directories(${HAVE_KDCHART})
  include_directories(${HAVE_KDCHART}/../src)
  link_directories("${HAVE_KDCHART}/../lib")
endif (HAVE_KDCHART)

# QT check
FIND_PATH( HAVE_QT Qt PATHS /opt/local/include /usr/include/qt4 )
if ( HAVE_QT )
    MESSAGE("Using ${HAVE_QT} for QT")
else ( HAVE_QT )
    MESSAGE("Not using QT library")
endif (HAVE_QT)

# Google Test - http://code.google.com/p/googletest/downloads/list
FIND_LIBRARY ( HAVE_GTEST gtest PATHS /usr/lib64 /usr/local/lib64 )
if ( HAVE_GTEST )
    MESSAGE("Using ${HAVE_GTEST} for Google Test")
else ( HAVE_GTEST )
    MESSAGE("Not using the Google Test library")
endif (HAVE_GTEST)

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
  ADD_SUBDIRECTORY(extern)
  
if (CMAKE_BUILD_TYPE STREQUAL DEBUG)

ADD_SUBDIRECTORY(testing)	# Test module and ctest stuff

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


