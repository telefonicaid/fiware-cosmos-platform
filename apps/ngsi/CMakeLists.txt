SET (SOURCES
    main_ngsi.cpp
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

ADD_EXECUTABLE(ngsi ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(ngsi ${STATIC_LIBS})

INSTALL(TARGETS               ngsi
        RUNTIME DESTINATION   bin)

