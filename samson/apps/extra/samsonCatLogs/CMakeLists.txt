SET (SOURCES
  main_samsonCatLogs.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     au
     lm
     pthread
     ${SOLARIS_LIBS}
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonCatLogs ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonCatLogs ${STATIC_LIBS})

INSTALL(TARGETS               samsonCatLogs
        RUNTIME DESTINATION   bin)

