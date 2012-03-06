SET (SOURCES
    tektronix_file_splitter.cpp
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

ADD_EXECUTABLE(tektronix_file_splitter ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(tektronix_file_splitter ${STATIC_LIBS} )

#INSTALL(TARGETS               tektronix_file_splitter
        #RUNTIME DESTINATION   bin)

