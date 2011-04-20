SET (HEADERS
	logMsg.h
	traceLevels.h
)

SET (SOURCES
    logMsg.cpp
)

# Inclue directories
# -----------------------------------------------------------------


# Library declaration
# -----------------------------------------------------------------

ADD_LIBRARY(lm STATIC ${SOURCES} ${HEADERS} )

