SET (SOURCES
    main_Delilah.cpp
)

SET (HEADERS
)

SET (STATIC_LIBS
     samsonNetworkStatic
     samsonCoreStatic
     samsonCommonStatic
     delilahStatic
     pa
     lm
     protobuf
     pthread
     dl
     curses
     readline
)


include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/core")
include_directories("${PROJECT_SOURCE_DIR}/libs/delilah")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")


link_directories("${PROJECT_BINARY_DIR}/libs/parseArgs")
link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")

# SAMSON Executable
ADD_EXECUTABLE(delilah ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(delilah ${STATIC_LIBS} )

INSTALL(TARGETS               delilah
        RUNTIME DESTINATION   bin)
