SET (HEADERS
	Select.h
	Table.h
	Tree.h
	Collection.h
	DataBase.h
    pugi.h
    pugixml.hpp
    pugiconfig.hpp
)

SET (SOURCES
	Select.cpp
	Table.cpp
	Tree.cpp
	Collection.cpp
	DataBase.cpp
    pugi.cpp
    pugixml.cpp
)

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(tables STATIC ${SOURCES} ${HEADERS})
INSTALL(TARGETS tables ARCHIVE DESTINATION lib)

