SET (HEADERS
Board.h
)

SET (SOURCES
Board.cpp
)

# Inclue directories
# -----------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(au_c4 STATIC ${SOURCES} ${HEADERS} )



