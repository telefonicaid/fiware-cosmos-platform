SET (SOURCES
    main_simple_mobility_generator.cpp
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

ADD_EXECUTABLE(simple_mobility_generator ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(simple_mobility_generator ${STATIC_LIBS})

INSTALL(TARGETS               simple_mobility_generator
        RUNTIME DESTINATION   bin)

