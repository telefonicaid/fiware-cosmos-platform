SET (SOURCES
  main_samsonDataSerializer.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
  pa
  ${SOLARIS_LIBS}
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonDataSerializer ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonDataSerializer ${STATIC_LIBS})

INSTALL(TARGETS               samsonDataSerializer
        RUNTIME DESTINATION   bin)

