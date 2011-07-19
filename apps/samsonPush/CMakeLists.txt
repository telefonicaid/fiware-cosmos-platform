SET (SOURCES
    main_samsonPush.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     samsonClient
     protobuf
     au
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonPush ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonPush ${STATIC_LIBS})

INSTALL(TARGETS               samsonPush
        RUNTIME DESTINATION   bin)

