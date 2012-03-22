SET (SOURCES
    main_UnitTest.cpp

    #engine
    engine/readFile_test.cpp
    engine/object_test.cpp
    engine/objectsManager_test.cpp
    #engine/processManager_test.cpp
    engine/processItem_test.cpp
    engine/operationscontainer_test.cpp
    engine/buffer_test.cpp
    #engine/diskStatistics_test.cpp
    #engine/diskOperation_test.cpp
    #engine/diskManager_test.cpp
    engine/memoryManager_test.cpp
    engine/engine_test.cpp
)


find_path(GTEST_INCLUDE_DIR gtest/gtest.h
          PATHS /usr/includes 
          PATH_SUFFIXES gtest )

find_library(GTESTMAIN_LIBRARY gtest_main
             PATHS /usr/lib64 /usr/local/lib64)

SET (HEADERS
     ${GTEST_INCLUDE_DIR}
)

SET (STATIC_LIBS
     ${HAVE_GTEST}
     ${GTESTMAIN_LIBRARY}
     engine
     pa
     lm
     protobuf
     pthread
     dl
     curses
     samsonNetwork
     samsonStream
     samsonModule
     samsonWorkerLib
     tables
     samsonCommon
     au
     xmlmarkup
     xmlparser	
     ${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/extern")

# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(unitTest ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(unitTest ${STATIC_LIBS} )

#INSTALL(TARGETS               unitTest
#        RUNTIME DESTINATION   bin)

ADD_TEST (unitTests ${PROJECT_BINARY_DIR}/apps/unitTest/unitTest --gtest_output=xml:${PROJECT_BINARY_DIR}/samson_test.xml
)
