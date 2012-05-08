SET (SOURCES
    main_ngsi.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     lm
     pa
	 json
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("/usr/local/include/json")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(ngsi ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(ngsi ${STATIC_LIBS})

INSTALL(TARGETS               ngsi
        RUNTIME DESTINATION   bin)

