SET (SOURCES
    arcanumConsumer.cpp
)

SET (HEADERS
)

SET (LIBS
    pa
    lm
au
)

include_directories("${PROJECT_SOURCE_DIR}/libs")


link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/parseArgs")

# SAMSON Executable
ADD_EXECUTABLE(arcanumConsumer ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(arcanumConsumer ${LIBS})

INSTALL(TARGETS               arcanumConsumer
        RUNTIME DESTINATION   bin)
