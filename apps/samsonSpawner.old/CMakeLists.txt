SET (SOURCES
    main_samsonSpawner.cpp
	processList.cpp
)

SET (HEADERS
	processList.h
)

SET (LIBS
    samsonNetwork
    engine
    pa
    lm
    au
    pthread
    protobuf
    samsonCommon
    ${COMMON_SAMSON_LIBS}
)

include_directories("${PROJECT_SOURCE_DIR}/libs")


link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/parseArgs")

# SAMSON Executable
ADD_EXECUTABLE(samsonSpawner ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonSpawner ${LIBS})

INSTALL(TARGETS               samsonSpawner
        RUNTIME DESTINATION   bin)
