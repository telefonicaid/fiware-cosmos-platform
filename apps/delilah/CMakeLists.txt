SET (SOURCES
    main_Delilah.cpp
)

SET (HEADERS
)

SET (STATIC_LIBS
     delilahLib
     pa
     lm
     protobuf
     pthread
     dl
     curses
     readline
     samsonNetwork
     samsonStream 
     engine
     au
     samsonCommon	
     ${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/delilah")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")


# Executable declaration
# ------------------------------------------------------------
ADD_EXECUTABLE(delilah ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(delilah ${STATIC_LIBS} )

INSTALL(TARGETS               delilah
        RUNTIME DESTINATION   bin)
