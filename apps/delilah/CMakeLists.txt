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
     curses
     samsonNetwork
     samsonStream
     samsonModule
     dl
     engine
     samsonCommon	
	 samson_system
     au
     ${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------
ADD_EXECUTABLE(delilah ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(delilah ${STATIC_LIBS} )

INSTALL(TARGETS               delilah
        RUNTIME DESTINATION   bin)
