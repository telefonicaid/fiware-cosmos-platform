SET (HEADERS
xmlParser.h
)

SET (SOURCES
xmlParser.cpp
)


# Include directories
# -----------------------------------------------------------------
#include_directories("${PROJECT_SOURCE_DIR}/libs")



# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(xmlparser STATIC ${SOURCES} ${HEADERS})
