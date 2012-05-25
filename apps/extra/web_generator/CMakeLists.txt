SET (SOURCES
    main_web_generator.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     au
     pa
     lm
     au
     dl
     pthread
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(web_generator ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(web_generator ${STATIC_LIBS})

INSTALL(TARGETS               web_generator
        RUNTIME DESTINATION   bin)

