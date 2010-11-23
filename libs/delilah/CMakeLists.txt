find_package(Qt4 COMPONENTS QtCore QtGui QtSvg REQUIRED )

IF(QT4_FOUND)
	INCLUDE( ${QT_USE_FILE} )
	
	SET (SOURCES
		Delilah.cpp
		DelilahLoadDataProcess.cpp
		DelilahConsole.cpp
		DelilahQt.cpp
		DelilahQtApp.cpp
		MainWindow.cpp
		Workspace.cpp
		WorkspaceScene.cpp
		WorkspaceView.cpp
		gui/InfoBox.cpp
		gui/CreateDataQueueDlg.cpp
		gui/LoadExistingQueueDlg.cpp
		Process.cpp
		items/ObjectItem.cpp
		items/QueueItem.cpp
		items/OperationItem.cpp
		items/ConnectionItem.cpp)
	
	SET (HEADERS
		Delilah.h
		DelilahLoadDataProcess.h
		DelilahClient.h
		DelilahConsole.h
		DelilahQt.h
		)
	
	SET (HEADERS_MOC
		DelilahQtApp.h
		MainWindow.h
		Workspace.h
		WorkspaceScene.h
		WorkspaceView.h
		gui/InfoBox.h
		gui/CreateDataQueueDlg.h
		gui/LoadExistingQueueDlg.h
		Queue.h
		Operation.h
//		DataType.h
		Process.h
		items/ObjectItem.h
		items/QueueItem.h
		items/OperationItem.h
		items/ConnectionItem.h
		Misc.h)
	
	SET (FORMS
		MainWindow.ui
		gui/forms/InfoBox.ui
		gui/forms/CreateDataQueueDlg.ui
		gui/forms/LoadExistingQueueDlg.ui)
	
	SET (RESOURCES
		resources/objects.qrc
		resources/icons.qrc)

	SET (STATIC_LIBS
		samsonNetworkStatic
		samsonModuleStatic
		samsonCommonStatic
		samsonCodingStatic
		samsonMemoryStatic
		samsonNetworkStatic
		samsonDiskStatic
		samsonDataStatic
		protobuf
		dl
	    pthread
		ncurses
	    )	

	QT4_WRAP_CPP(MOC_SOURCES ${HEADERS_MOC})
	QT4_WRAP_UI(FORMS_HEADERS ${FORMS})
	QT4_ADD_RESOURCES(RESOURCES_QRC ${RESOURCES})
	
	include_directories(${CMAKE_CURRENT_BINARY_DIR})
	include_directories(${CMAKE_CURRENT_SOURCE_DIR})
	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/items")
	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/gui")
	include_directories("${PROJECT_SOURCE_DIR}/libs/core")
	include_directories("${PROJECT_SOURCE_DIR}/libs/network")
	include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
	include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
	include_directories("${PROJECT_BINARY_DIR}/libs/network")
	include_directories("${PROJECT_SOURCE_DIR}/libs/common")
	include_directories("${PROJECT_SOURCE_DIR}/libs/module")
	include_directories("${PROJECT_SOURCE_DIR}/libs/coding")
	include_directories("${PROJECT_SOURCE_DIR}/libs/disk")
	include_directories("${PROJECT_SOURCE_DIR}/libs/data")

	ADD_LIBRARY(delilahStatic ${SOURCES} ${HEADERS} ${MOC_SOURCES} ${FORMS_HEADERS} ${RESOURCES_QRC})
	TARGET_LINK_LIBRARIES(delilahStatic ${QT_LIBRARIES} ${STATIC_LIBS})

ELSE(QT4_FOUND)
	MESSAGE( STATUS "Qt4 not found - skipping compilation of delilah_lib")
ENDIF(QT4_FOUND)



