SET (HEADERS
	logMsg.h
	traceLevels.h
)

SET (SOURCES
    logMsg.cpp
)

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")

# Library declaration
# -----------------------------------------------------------------

ADD_LIBRARY(lm STATIC ${SOURCES} ${HEADERS} )
SET (LIBLM ${PROJECT_BINARY_DIR}/libs/LogMsg/liblm.a CACHE INTERNAL "")

INSTALL(TARGETS lm ARCHIVE DESTINATION lib)
