SET (SOURCES
    main_SamsonSetup.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     samsonNetwork
     engine
     samsonModule
     samsonCommon
     protobuf
     pthread
     au
     dl
     lm
	au
	 pa
     ${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")

# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonSetup ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonSetup ${STATIC_LIBS})

INSTALL(TARGETS               samsonSetup
        RUNTIME DESTINATION   bin)

