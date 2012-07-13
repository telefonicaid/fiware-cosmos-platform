
SET (HEADERS
	DelilahBase.h
	DelilahBaseConnection.h
	Delilah.h
	DelilahConsole.h
	DelilahComponent.h
	WorkerCommandDelilahComponent.h
	PushDelilahComponent.h
	PopDelilahComponent.h
	RepeatDelilahComponent.h
	TXTFileSet.h
	SamsonDataSet.h
	SamsonFile.h
	DelilahCommandCatalogue.h
	AlertCollection.h
)

SET (SOURCES
	Delilah.cpp
	DelilahBase.cpp
	DelilahBaseConnection.cpp	
	DelilahConsole.cpp
	DelilahComponent.cpp
	WorkerCommandDelilahComponent.cpp
	PushDelilahComponent.cpp
	PopDelilahComponent.cpp
	RepeatDelilahComponent.cpp
	TXTFileSet.cpp
	SamsonDataSet.cpp
	SamsonFile.cpp
	DelilahCommandCatalogue.cpp
	AlertCollection.cpp
)
	
	
# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(delilahLib ${SOURCES} ${HEADERS})
