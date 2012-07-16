
SET (HEADERS
	DelilahQt.h
	DelilahMainWindow.h
	QueueViewer.h
	QueueContainer.h
	ExtQueueViewer.h
	WorkerViewer.h
	WorkerContainer.h
	DataViewPlugin.h
	LineViewPlugin.h
	Plot.h
	BarPlot.h
	LinePlot.h
)

SET (SOURCES
	DelilahQt.cpp
	DelilahMainWindow.cpp
	TableViewer.cpp
	QueueViewer.cpp
	QueueContainer.cpp
	ExtQueueViewer.cpp
	WorkerViewer.cpp
	WorkerContainer.cpp
	DataViewPlugin.cpp
	LineViewPlugin.cpp
	BarPlot.cpp
	LinePlot.cpp
)

SET (HEADERS_FOR_MOC
	DelilahQt.h
	DelilahMainWindow.h
	TableViewer.h
	QueueViewer.h
	QueueContainer.h
	ExtQueueViewer.h
	WorkerViewer.h
	WorkerContainer.h
	DataViewPlugin.h
	LineViewPlugin.h
	BarPlot.h
	LinePlot.h
)

FIND_PACKAGE(Qt4 REQUIRED)

SET (STATIC_LIBS
     QtCore
     kdchart
)

include_directories("${PROJECT_SOURCE_DIR}/libs")
#include_directories("/usr/local/KDAB/KDChart-2.3.0/src")
#include_directories("/usr/local/Trolltech/Qt-4.7.4/include")
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})		# Required for QT intermediate files to compile

# QT Operations
# ------------------------------------------------------------
QT4_WRAP_CPP( HEADERS_MOC ${HEADERS_FOR_MOC} )
#QT4_WRAP_UI(FORMS_HEADERS ${FORMS})
#QT4_ADD_RESOURCES(RESOURCES_RCC ${RESOURCES})
# ------------------------------------------------------------

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(delilahQtLib ${SOURCES} ${HEADERS} ${HEADERS_MOC})

TARGET_LINK_LIBRARIES(delilahQtLib ${STATIC_LIBS} )
TARGET_LINK_LIBRARIES(delilahQtLib ${QT_LIBRARIES} )

