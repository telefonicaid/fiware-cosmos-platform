SET (SOURCES
    main_UnitTest.cpp
)


find_path(GTEST_INCLUDE_DIR gtest/gtest.h
          PATHS /usr/includes 
          PATH_SUFFIXES gtest )

find_library(GTEST_LIBRARY libgtest
             PATHS /usr/lib64 )

find_library(GTESTMAIN_LIBRARY libgtest_main
             PATHS /usr/lib64 )

SET (HEADERS
     ${GTEST_INCLUDE_DIR}
)

SET (STATIC_LIBS
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
    ${GTEST_LIBRARY}
     ${GTESTMAIN_LIBRARY}
     ${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------
ADD_EXECUTABLE(unitTest ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(unitTest ${STATIC_LIBS} )

#INSTALL(TARGETS               unitTest
#        RUNTIME DESTINATION   bin)
