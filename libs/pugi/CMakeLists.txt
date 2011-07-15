SET (HEADERS
    pugi.h
    pugixml.hpp
)

SET (SOURCES
    pugi.cpp
    pugiconfig.hpp
    pugixml.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(pugi STATIC ${SOURCES} ${HEADERS})





