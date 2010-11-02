find_package(Qt4 COMPONENTS QtCore QtGui QtSvg REQUIRED )

IF(QT4_FOUND)
	INCLUDE( ${QT_USE_FILE} )
	
	SET (SOURCES
		DelilahQt.cpp
		MainWindow.cpp
		InfoBox.cpp
		ProcessView.cpp
		ProcessScene.cpp
		items/ObjectItem.cpp
		items/QueueItem.cpp
		items/OperationItem.cpp
		items/ConnectionItem.cpp)
	
	SET (HEADERS
		ProcessView.h)
	
	SET (HEADERS_MOC
		MainWindow.h
		InfoBox.h
		ProcessScene.h
		items/ObjectItem.h
		items/QueueItem.h
		items/OperationItem.h
		items/ConnectionItem.h)
	
	SET (FORMS
		MainWindow.ui
		InfoBox.ui)
	
	SET (RESOURCES
		resources/objects.qrc
		resources/icons.qrc)
	
	QT4_WRAP_CPP(MOC_SOURCES ${HEADERS_MOC})
	QT4_WRAP_UI(FORMS_HEADERS ${FORMS})
	QT4_ADD_RESOURCES(RESOURCES_QRC ${RESOURCES})
	
	include_directories(${CMAKE_CURRENT_BINARY_DIR})
	include_directories(${CMAKE_CURRENT_SOURCE_DIR})
	include_directories("${CMAKE_CURRENT_SOURCE_DIR}/items")
	
	# SAMSON Executable
	ADD_LIBRARY(delilah_qt ${SOURCES} ${HEADERS} ${MOC_SOURCES} ${FORMS_HEADERS} ${RESOURCES_QRC})
	TARGET_LINK_LIBRARIES(delilah_qt ${QT_LIBRARIES})
ELSE(QT4_FOUND)
	MESSAGE( STATUS "Qt4 not found - skipping compilation of DelilahQt")
ENDIF(QT4_FOUND)
