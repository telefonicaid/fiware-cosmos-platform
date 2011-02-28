SET (HEADERS
    coding.h

)

SET (SOURCES
    coding.cpp
)

# Inclue directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/disk")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonCoding STATIC ${SOURCES} ${HEADERS} )
