SET (SOURCES
    parse_tektronix_file_standalone.cpp
    ../tektronix_data.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     au
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(parse_tektronix_file_standalone ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(parse_tektronix_file_standalone ${STATIC_LIBS} )

#INSTALL(TARGETS               parse_tektronix_file_standalone
        #RUNTIME DESTINATION   bin)

