SET (SOURCES
    main_samsonSupervisor.cpp
	baTerm.cpp
)

SET (HEADERS
)


include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")

link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/parseArgs")

# SAMSON Executable
ADD_EXECUTABLE(samsonSupervisor ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonSupervisor samsonNetworkStatic pa lm pthread)

INSTALL(TARGETS               samsonSupervisor
        RUNTIME DESTINATION   bin)
