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
	actions.cpp
	TabManager.cpp
	DelilahTab.cpp
	LogTab.cpp
	ProcessListTab.cpp
	configFile.cpp
	Popup.cpp
	ConfigWindow.cpp
	SceneTab.cpp
	misc.cpp
	DelilahQueue.cpp
	DelilahScene.cpp
	DelilahConnection.cpp
	ConnectionMgr.cpp
	QueueMgr.cpp
)

SET (HEADERS_MOC
	Starter.h
	TabManager.h
	DelilahTab.h
	LogTab.h
	ProcessListTab.h
	ConfigWindow.h
	Popup.h
	SceneTab.h
	DelilahScene.h
)

SET (HEADERS
	SamsonSupervisor.h
	baTerm.h
	Process.h
	processList.h
	spawnerList.h
	starterList.h
	actions.h
	globals.h
	configFile.h
	misc.h
	DelilahQueue.h
	DelilahConnection.h
	ConnectionMgr.h
	QueueMgr.h
)


SET (RESOURCES
    ${PROJECT_SOURCE_DIR}/apps/samsonSupervisor/samsonSupervisor.qrc)

QT4_ADD_RESOURCES(RESOURCES_QRC ${RESOURCES})

QT4_WRAP_CPP(MOC_SOURCES ${HEADERS_MOC})

include_directories("/usr/include/qt4")
include_directories("/usr/include/qt4/QtGui")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/delilah")

link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/parseArgs")
link_directories("${PROJECT_BINARY_DIR}/libs/delilah")

# SAMSON Executable
ADD_EXECUTABLE(samsonSupervisor ${SOURCES} ${MOC_SOURCES} ${HEADERS} ${RESOURCES_QRC} )
TARGET_LINK_LIBRARIES(samsonSupervisor samsonNetworkStatic delilahStatic pa lm readline pthread ${QT_LIBRARIES})

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
