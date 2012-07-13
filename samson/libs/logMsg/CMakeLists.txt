SET (HEADERS
	logMsg.h
	traceLevels.h
	time.h
)

SET (SOURCES
    logMsg.cpp
	time.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")

# Library declaration
# -----------------------------------------------------------------

ADD_LIBRARY(lm STATIC ${SOURCES} ${HEADERS} )
SET (LIBLM ${PROJECT_BINARY_DIR}/libs/logMsg/liblm.a CACHE INTERNAL "")

# Only needed for the "samson" project
IF (CMAKE_PROJECT_NAME STREQUAL "samson")
    INSTALL(TARGETS lm ARCHIVE DESTINATION lib)
    INSTALL(FILES ${HEADERS} DESTINATION include/logMsg)
ENDIF (CMAKE_PROJECT_NAME STREQUAL "samson")
