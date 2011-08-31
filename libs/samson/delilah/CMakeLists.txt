
SET (HEADERS
	Delilah.h
	DelilahUtils.h
	DelilahConsole.h
	DelilahMonitorization.h
	DelilahClient.h
	DelilahComponent.h
	DelilahDownloadDataProcess.h
	DelilahUploadDataProcess.h
	PushComponent.h
	TXTFileSet.h
)

SET (SOURCES
	Delilah.cpp
	DelilahUtils.cpp
	DelilahConsole.cpp
	DelilahMonitorization.cpp
	DelilahComponent.cpp
	DelilahDownloadDataProcess.cpp
	DelilahUploadDataProcess.cpp
	PushComponent.cpp
	TXTFileSet.cpp
)
	
	
# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(delilahLib ${SOURCES} ${HEADERS})
