SET (HEADERS
xmlmarkup.h
)

SET (SOURCES
xmlmarkup.cpp
)


# Include directories
# -----------------------------------------------------------------
#include_directories("${PROJECT_SOURCE_DIR}/libs")



# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(xmlmarkup STATIC ${SOURCES} ${HEADERS})
