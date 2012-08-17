
SET (HEADERS
	DelilahBase.h
	Delilah.h
	PushManager.h
	DelilahConsole.h
	DelilahComponent.h
	PushDelilahComponent.h
	PopDelilahComponent.h
	WorkerCommandDelilahComponent.h
	TXTFileSet.h
	DelilahCommandCatalogue.h
	AlertCollection.h
)

SET (SOURCES
	DelilahBase.cpp
	Delilah.cpp
	PushManager.cpp
	DelilahConsole.cpp
	DelilahComponent.cpp
	PushDelilahComponent.cpp
	PopDelilahComponent.cpp
	WorkerCommandDelilahComponent.cpp
	TXTFileSet.cpp
	DelilahCommandCatalogue.cpp
	AlertCollection.cpp
)
	
	
# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(delilahLib ${SOURCES} ${HEADERS})
