SET (SOURCES
    main_UnitTest.cpp
    buffer_test.cpp
    diskOperation_test.cpp
    diskManager_test.cpp
    engine_test.cpp
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
     pugi
     au
     samsonCommon
     xmlmarkup	
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
