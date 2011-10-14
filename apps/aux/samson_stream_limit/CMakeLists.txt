SET (SOURCES
    main_samson_stream_limit.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     au
     lm
     pa
     dl
     pthread
     readline
     curses
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samson_stream_limit ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samson_stream_limit ${STATIC_LIBS})

INSTALL(TARGETS               samson_stream_limit
        RUNTIME DESTINATION   bin)

