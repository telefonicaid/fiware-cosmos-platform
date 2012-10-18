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
ADD_LIBRARY( au_c4 STATIC ${SOURCES} ${HEADERS} )


#Install headers
INSTALL(TARGETS au_c4 ARCHIVE DESTINATION lib)
INSTALL(FILES ${HEADERS} DESTINATION include/c4)
