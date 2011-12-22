SET (HEADERS
	TableCollection.h
)

SET (SOURCES
	TableCollection.cpp
)

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(tables STATIC ${SOURCES} ${HEADERS})
INSTALL(TARGETS tables ARCHIVE DESTINATION lib)

