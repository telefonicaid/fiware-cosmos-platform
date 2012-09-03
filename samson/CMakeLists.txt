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

# Baseline compiler flags, any change here will affect all build types
set (CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas -D_LARGEFILE64_SOURCE -D_GNU_SOURCE -Werror")

#
# Platform checks
# 
MESSAGE( "Compiling in system ${CMAKE_SYSTEM_NAME}")

IF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    ADD_DEFINITIONS(-DOSX)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DOSX")

	# Several compilers are used in mac environment
	#
	# gcc 4.2  : Default in current Lion version
	# clang    : Usefull for code-coverage
	# gcc 4.7  : from mac ports

    # By default if CC or CXX are set to default gcc 4.2
    # to override this set CC or CXX to something other than /usr/bin/gcc or /usr/bin/g++
    #
    # e.g. CC=gcc-mp-4.7 CXX=g++-mp-4.7 make .....
    # e.g. CC=clang  CXX=clang++ make .....

	MESSAGE ("Using ${CMAKE_C_COMPILER} as our C compiler")
	MESSAGE ("Using ${CMAKE_CXX_COMPILER} as our C++ compiler")

ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    ADD_DEFINITIONS(-DLINUX)
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLINUX")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# Disable unused but set variables - GCC 4.7+
IF (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-but-set-variable")
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")

# ---------------------------------------------------------------------------
#
# DEBUG
#
# RELEASE
#
# ---------------------------------------------------------------------------

if (CMAKE_BUILD_TYPE STREQUAL DEBUG)

    MESSAGE("cmake: DEBUG compilation")
    add_definitions(-DDEBUG -DDEBUG_$ENV{USER} -DDEBUG_${USER})
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
    if (COVERAGE)
        MESSAGE("cmake: Coverage compilation")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
        set (CMAKE_EXE_LINKER_FLAGS "-fprofile-arcs -ftest-coverage")
    endif (COVERAGE)

elseif (CMAKE_BUILD_TYPE STREQUAL RELEASE)

    MESSAGE("cmake: RELEASE compilation")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")

    if (STRICT)
        MESSAGE("cmake: Strict compilation")
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
    endif (STRICT)

else (CMAKE_BUILD_TYPE STREQUAL DEBUG)
    MESSAGE("cmake: bad value for CMAKE_BUILD_TYPE - please set the variable to either DEBUG or RELEASE")
    set(error 1)
endif (CMAKE_BUILD_TYPE STREQUAL DEBUG)

# -Wunused-but-set-variable is available from gcc 4.6.0 but can be set in 4.5.x
# OS X comes with gcc 4.2.1 (10.7) and does not accept this flag
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-but-set-variable")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

#
# Enabling testing in debug mode
#
if (CMAKE_BUILD_TYPE STREQUAL DEBUG)
   ENABLE_TESTING()
   INCLUDE(Dart)
   SET (CMAKE_TEST_TIMEOUT 60)
   SET (DART_TIMEOUT 60)
   ADD_SUBDIRECTORY(test)
endif (CMAKE_BUILD_TYPE STREQUAL DEBUG)


#
# Common include
#

include_directories("/opt/local/include")
include_directories("/opt/local/include/json")
include_directories("/usr/local/include")
include_directories("/usr/include")
include_directories("/usr/local/include/json")
include_directories("/usr/include/json")
include_directories("/usr/include/x86_64-linux-gnu") # Ubuntu 12.04
include_directories("/usr/local/include/c-client-src")

#
# Library directories
#

link_directories("/opt/local/lib")
link_directories("/usr/local/lib")
link_directories("/usr/lib64")
link_directories("/usr/lib/x86_64-linux-gnu") # Ubuntu 12.04

if (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")
link_directories("/usr/local/lib/amd64")
endif (${CMAKE_SYSTEM_NAME} MATCHES "SunOS")

# ---------------------------------------------------------
# External dependency checks
# ---------------------------------------------------------

# See if we have KDChart installed (check_include_files does not work due to these being C++ headers)
FIND_PATH(HAVE_KDCHART KDChartChart PATHS /usr/local/KDAB/KDChart-2.3.0/include /usr/local/kdchart/include /usr/local/KDAB/KDChart-2.4.2/include)
if (HAVE_KDCHART)
  MESSAGE("Using ${HAVE_KDCHART} for KDChart")
  include_directories(${HAVE_KDCHART})
  include_directories(${HAVE_KDCHART}/../src)
  link_directories("${HAVE_KDCHART}/../lib")
endif (HAVE_KDCHART)

## QT check
#FIND_PATH( HAVE_QT Qt PATHS /opt/local/include /usr/include/qt4 )
#if ( HAVE_QT )
#    MESSAGE("Using ${HAVE_QT} for QT")
#else ( HAVE_QT )
#    MESSAGE("Not using QT library")
#endif (HAVE_QT)

# Google Test - http://code.google.com/p/googletest/downloads/list
FIND_LIBRARY ( HAVE_GTEST gtest PATHS /usr/lib /usr/local/lib /usr/lib64 /usr/local/lib64 )
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
# Indicate this is an SVN build
set (SVN_BUILD TRUE)

#
# Continuing if OK
#
if (error EQUAL 0)
  MESSAGE("cmake: OK")
  ADD_SUBDIRECTORY(scripts)
  ADD_SUBDIRECTORY(libs)
  ADD_SUBDIRECTORY(apps)
  ADD_SUBDIRECTORY(extern)
  if (BUILD_MODULES)
    MESSAGE("Building platform analytical modules")
    ADD_SUBDIRECTORY(modules)
  endif (BUILD_MODULES)

  
else ()
    MESSAGE("cmake: NOT OK")
endif (error EQUAL 0)


