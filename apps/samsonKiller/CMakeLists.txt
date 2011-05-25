SET (SOURCES
    main_samsonKiller.cpp
)

SET (HEADERS
)

SET (LIBS
samsonNetwork
engine
samsonCommon
au
pa
lm
pthread	
protobuf
${COMMON_SAMSON_LIBS}
)

include_directories("${PROJECT_SOURCE_DIR}/libs")


link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/parseArgs")

# SAMSON Executable
ADD_EXECUTABLE(samsonKiller ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonKiller ${LIBS})

INSTALL(TARGETS               samsonKiller
        RUNTIME DESTINATION   bin)
