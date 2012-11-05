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
TARGET_LINK_LIBRARIES( zoo zookeeper_mt lm pa au engine)
INSTALL(TARGETS zoo DESTINATION lib)
