find_package(Qt4 COMPONENTS QtCore QtGui QtSvg REQUIRED )
INCLUDE(${QT_USE_FILE})

SET (SOURCES
	main_samsonSupervisor.cpp
	SamsonSupervisor.cpp
	baTerm.cpp
	processList.cpp
	spawnerList.cpp
	Starter.cpp
	starterList.cpp
	TabManager.cpp
	
	ProcessListTab.cpp
	MrOperationsTab.cpp
	LogTab.cpp
	DelilahRawTab.cpp
	ConfigTab.cpp
	OldMrOperationsTab.cpp
	
	Popup.cpp
	misc.cpp
	DelilahSceneItem.cpp
	DelilahQueue.cpp
	DelilahScene.cpp
	DelilahConnection.cpp
	ConnectionMgr.cpp
	QueueMgr.cpp
	SourceMgr.cpp
	ResultMgr.cpp
	QueueConfigWindow.cpp
	ResultConfigWindow.cpp
	SourceConfigWindow.cpp
	permissions.cpp
	UserMgr.cpp
	LoginWindow.cpp
	DelilahSource.cpp
	DelilahResult.cpp
	ProcessConfigView.cpp
	InfoWin.cpp
	LogFileWindow.cpp
)

SET (HEADERS_MOC
	TabManager.h

	ProcessListTab.h
	MrOperationsTab.h
	LogTab.h
	DelilahRawTab.h
	ConfigTab.h
	OldMrOperationsTab.h

	Starter.h
	Popup.h
	DelilahScene.h
	QueueConfigWindow.h
	ResultConfigWindow.h
	SourceConfigWindow.h
	LoginWindow.h
	ProcessConfigView.h
	InfoWin.h
	LogFileWindow.h
)

SET (HEADERS
	SamsonSupervisor.h
	baTerm.h
	processList.h
	spawnerList.h
	starterList.h
	globals.h
	misc.h
	DelilahSceneItem.h
	DelilahQueue.h
	DelilahSource.h
	DelilahResult.h
	DelilahConnection.h
	ConnectionMgr.h
	QueueMgr.h
	SourceMgr.h
	ResultMgr.h
	UserMgr.h
	permissions.h
	ResultMgr.h
	SourceMgr.h
)

SET (LIBS
	delilahLib 
	samsonNetwork 
	samsonMemory
	samsonCommon
	z
	pa 
	lm 
	readline 
	pthread
	protobuf
	${COMMON_SAMSON_LIBS}
)

SET (RESOURCES
    ${PROJECT_SOURCE_DIR}/apps/samsonSupervisor/samsonSupervisor.qrc)

QT4_ADD_RESOURCES(RESOURCES_QRC ${RESOURCES})

QT4_WRAP_CPP(MOC_SOURCES ${HEADERS_MOC})

include_directories("/usr/include/qt4")
include_directories("/usr/include/qt4/QtGui")
include_directories("${PROJECT_SOURCE_DIR}/libs")

# SAMSON Executable
ADD_EXECUTABLE(samsonSupervisor ${SOURCES} ${MOC_SOURCES} ${HEADERS} ${RESOURCES_QRC} )
TARGET_LINK_LIBRARIES(samsonSupervisor ${LIBS} ${QT_LIBRARIES})

install(TARGETS                samsonSupervisor
        RUNTIME  DESTINATION   bin)
#		DIRECTORY			   images/ DESTINATION   share)

# images.path     = /usr/local/samson/images
# images.files   += images/queue.png
# images.files   += images/queueDelete.png
# images.files   += images/queueAdd128x128.png
# images.files   += images/connect.gif
# images.files   += images/queueDelete.png
# INSTALLS       += images

# target.path += $$[PROJECT_SOURCE_DIR]/images
# INSTALLS += target

