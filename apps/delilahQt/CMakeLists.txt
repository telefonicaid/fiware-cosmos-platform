find_package(Qt4 COMPONENTS QtCore QtGui QtSvg REQUIRED )

INCLUDE( ${QT_USE_FILE} )

SET (SOURCES
	DelilahQt.cpp
	MainWindow.cpp
	ProcessView.cpp
	ProcessScene.cpp
	QueueItem.cpp)

SET (HEADERS
	)

SET (HEADERS_MOC
	MainWindow.h
	ProcessView.h
	ProcessScene.h
	QueueItem.h)

SET (FORMS
	MainWindow.ui)

SET (RESOURCES
	resources/objects.qrc
	resources/icons.qrc)

QT4_WRAP_CPP(MOC_SOURCES ${HEADERS_MOC})
QT4_WRAP_UI(FORMS_HEADERS ${FORMS})
QT4_ADD_RESOURCES(RESOURCES_QRC ${RESOURCES})

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# SAMSON Executable
ADD_EXECUTABLE(delilah_qt ${SOURCES} ${HEADERS} ${MOC_SOURCES} ${FORMS_HEADERS} ${RESOURCES_QRC})
TARGET_LINK_LIBRARIES(delilah_qt ${QT_LIBRARIES})
