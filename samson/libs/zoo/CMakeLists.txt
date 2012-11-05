SET (HEADERS
common.h
Connection.h
ZooNodeCommiter.h
)

SET (SOURCES
common.cpp
Connection.cpp
ZooNodeCommiter.cpp
)

# Inclue directories
# -----------------------------------------------------------------

include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(zoo SHARED ${SOURCES})
INSTALL(TARGETS zoo DESTINATION lib)
