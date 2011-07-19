
SET (HEADERS
	SamsonClient.h
)

SET (SOURCES
	SamsonClient.cpp
)

	
# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonClient STATIC ${SOURCES} ${HEADERS})


INSTALL(TARGETS samsonClient  DESTINATION   lib)
