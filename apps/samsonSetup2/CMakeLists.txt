SET (SOURCES
    main_samsonSetup.cpp
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
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")


link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/parseArgs")

# SAMSON Executable
ADD_EXECUTABLE(samsonSetup2 ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonSetup2 ${LIBS})

INSTALL(TARGETS               samsonSetup2
        RUNTIME DESTINATION   bin)
