SET (SOURCES
    main_word_generator.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     au
     lm
     pa
     dl
     pthread
     edit
     curses
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(word_generator ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(word_generator ${STATIC_LIBS})

INSTALL(TARGETS               word_generator
        RUNTIME DESTINATION   bin)

