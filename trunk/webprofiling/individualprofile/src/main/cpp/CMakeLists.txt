PROJECT(SamsonComscore)
cmake_minimum_required(VERSION 2.6)
set(THIRD_PARTY ../../../../../../third-party/)

#
# DEBUG or RELEASE build ?
#
if (NOT CMAKE_BUILD_TYPE)
    MESSAGE("No build type set (-DCMAKE_BUILD_TYPE=DEBUG|RELEASE), RELEASE by default")
    SET(CMAKE_BUILD_TYPE RELEASE)
endif (NOT CMAKE_BUILD_TYPE)

set(error 0)

execute_process(COMMAND            echo $USER
                COMMAND            tr 'a-z' 'A-Z'
                OUTPUT_VARIABLE    USER)

add_definitions(-DLM_ON)
add_definitions(-fPIC)

# Baseline compiler flags, any change here will affect all build types
set (CMAKE_CXX_FLAGS "-Wall -Wno-unknown-pragmas -D_LARGEFILE64_SOURCE -D_GNU_SOURCE -Werror")

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

else (CMAKE_BUILD_TYPE STREQUAL DEBUG)
    MESSAGE("cmake: bad value for CMAKE_BUILD_TYPE - please set the variable to either DEBUG or RELEASE")
    set(error 1)
endif (CMAKE_BUILD_TYPE STREQUAL DEBUG)

# -Wunused-but-set-variable is available from gcc 4.6.0 but can be set in 4.5.x
# OS X comes with gcc 4.2.1 (10.7) and does not accept this flag
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-but-set-variable")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

SET(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE TRUE)

#
# Common include
#
include_directories("/opt/local/include")
include_directories("${THIRD_PARTY}/samson/include")
#
# Library directories
#
cmake_policy(SET CMP0015 NEW)
link_directories("/opt/local/lib")
link_directories("/usr/lib64")
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    link_directories("${THIRD_PARTY}/samson/linux/lib")
else (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    link_directories("${THIRD_PARTY}/samson/macosx/lib")
endif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

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
# Java tools
#
FIND_PACKAGE(JAVA REQUIRED)
FIND_PACKAGE(JNI REQUIRED)

#
# Continuing if OK
#
if (error EQUAL 0)
  MESSAGE("cmake: OK")

  ADD_CUSTOM_COMMAND(OUTPUT es_tid_bdp_profile_dictionary_comscore_CSDictionaryJNIInterface.h COMMAND ${Java_JAVAH_EXECUTABLE} -jni -classpath ../../../target/classes/ es.tid.bdp.profile.dictionary.comscore.CSDictionaryJNIInterface)
  INCLUDE_DIRECTORIES(${JNI_INCLUDE_DIRS})
  INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}/libs")
  ADD_LIBRARY(comscore SHARED libcomscore.cpp es_tid_bdp_profile_dictionary_comscore_CSDictionaryJNIInterface.h)
  TARGET_LINK_LIBRARIES(comscore tables samson_comscore au lm dl pthread)

  
if (CMAKE_BUILD_TYPE STREQUAL DEBUG)

endif (CMAKE_BUILD_TYPE STREQUAL DEBUG)

else ()
    MESSAGE("cmake: NOT OK")
endif (error EQUAL 0)
