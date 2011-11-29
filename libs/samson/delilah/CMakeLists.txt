
SET (HEADERS
	DelilahBase.h
	Delilah.h
	DelilahUtils.h
	DelilahConsole.h
	DelilahClient.h
	DelilahComponent.h
	CommandDelilahComponent.h
	WorkerCommandDelilahComponent.h
	PushDelilahComponent.h
	PopDelilahComponent.h
	DownloadDelilahComponent.h
	UploadDelilahComponent.h
	TXTFileSet.h
	SamsonDataSet.h
	SamsonFile.h
)

SET (SOURCES
	Delilah.cpp
	DelilahBase.cpp
	DelilahUtils.cpp
	DelilahConsole.cpp
	DelilahComponent.cpp
	CommandDelilahComponent.cpp
	WorkerCommandDelilahComponent.cpp
	PushDelilahComponent.cpp
	PopDelilahComponent.cpp
	DownloadDelilahComponent.cpp
	UploadDelilahComponent.cpp
	TXTFileSet.cpp
	SamsonDataSet.cpp
	SamsonFile.cpp

)
	
	
# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(delilahLib ${SOURCES} ${HEADERS})
