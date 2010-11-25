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
elseif (CMAKE_BUILD_TYPE STREQUAL RELEASE)
    MESSAGE("cmake: RELEASE compilation")
    add_definitions(-DRELEASE)
else (CMAKE_BUILD_TYPE STREQUAL DEBUG)
    MESSAGE("cmake: bad value for CMAKE_BUILD_TYPE - please set the variable to either DEBUG or RELEASE")
    set(error 1)
endif (CMAKE_BUILD_TYPE STREQUAL DEBUG)


# set(CMAKE_CXX_FLAGS, "-fPIC")
# set(CMAKE_C_FLAGS, "-fPIC")


#
# Architecture 
#
set (CMAKE_OSX_ARCHITECTURES "x86_64" CACHE STRING "Rogue")



#
# Compilation flags
#
set (CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas -D_LARGEFILE64_SOURCE -D_GNU_SOURCE -g -Werror")



#
# Continuing if OK
#
if (error EQUAL 0)
  MESSAGE("cmake: OK")
  ADD_SUBDIRECTORY(libs)
  ADD_SUBDIRECTORY(apps)
  ADD_SUBDIRECTORY(etc)
else ()
    MESSAGE("cmake: NOT OK")
endif (error EQUAL 0)
