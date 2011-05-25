
SET (SOURCES
	Delilah.cpp
	DelilahConsole.cpp
	DelilahComponent.cpp
	DelilahDownloadDataProcess.cpp
	DelilahUploadDataProcess.cpp
	TXTFileSet.cpp
)
	
SET (HEADERS
	Delilah.h
	DelilahConsole.h
	DelilahClient.h
	DelilahComponent.h
	DelilahDownloadDataProcess.h
	DelilahUploadDataProcess.h
	TXTFileSet.h
)
	
# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(delilahLib ${SOURCES} ${HEADERS})
