SET (SOURCES
    main_SamsonTest.cpp
)

SET (HEADERS
)

SET( STATIC_LIBS
     ${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")

# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonTest ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(samsonTest ${STATIC_LIBS})

INSTALL(TARGETS               samsonTest
        RUNTIME DESTINATION   bin)

