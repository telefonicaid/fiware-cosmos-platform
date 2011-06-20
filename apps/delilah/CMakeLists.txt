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


# Executable declaration
# ------------------------------------------------------------
ADD_EXECUTABLE(delilah2 ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(delilah2 ${STATIC_LIBS} )

INSTALL(TARGETS               delilah2
        RUNTIME DESTINATION   bin)
