
SET (HEADERS
	DelilahQt.h
)

SET (SOURCES
	DelilahQt.cpp
)

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(delilahQtLib ${SOURCES} ${HEADERS})
