SET (SOURCES
    main_wiki_extractor.cpp
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

ADD_EXECUTABLE(wiki_extractor ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(wiki_extractor ${STATIC_LIBS})

INSTALL(TARGETS               wiki_extractor
        RUNTIME DESTINATION   bin)

