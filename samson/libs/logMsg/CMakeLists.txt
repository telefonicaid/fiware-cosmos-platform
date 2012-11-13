SET (HEADERS
logMsg.h
time.h
traceLevels.h
)

SET (SOURCES
logMsg.cpp
time.cpp
traceLevels.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")

# Library declaration
# -----------------------------------------------------------------

ADD_LIBRARY(lm SHARED ${SOURCES} ${HEADERS} )
#SET (LIBLM ${PROJECT_BINARY_DIR}/libs/logMsg/liblm.a CACHE INTERNAL "")

# Only needed for the "Samson" project
IF (CMAKE_PROJECT_NAME STREQUAL "Samson")
    INSTALL(TARGETS lm  DESTINATION lib)
    INSTALL(FILES ${HEADERS} DESTINATION include/logMsg)
ENDIF (CMAKE_PROJECT_NAME STREQUAL "Samson")

