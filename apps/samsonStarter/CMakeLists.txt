SET (SOURCES
    main_samsonStarter.cpp
    SamsonStarter.cpp
)

SET (HEADERS
    SamsonStarter.h
)

SET (LIBS
    samsonNetwork
    engine
    protobuf
    samsonCommon
    ${COMMON_SAMSON_LIBS}
    au
    pa
    lm
    pthread
)

include_directories("${PROJECT_SOURCE_DIR}/libs")


link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/parseArgs")

# SAMSON Executable
ADD_EXECUTABLE(samsonStarter ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonStarter ${LIBS})

INSTALL(TARGETS               samsonStarter
        RUNTIME DESTINATION   bin)
