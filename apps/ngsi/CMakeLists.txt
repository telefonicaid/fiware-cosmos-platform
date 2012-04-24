SET (SOURCES
    main_ngsiTest.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     lm
     pa
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(ngsiTest ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(ngsiTest ${STATIC_LIBS})

INSTALL(TARGETS               ngsiTest
        RUNTIME DESTINATION   bin)

