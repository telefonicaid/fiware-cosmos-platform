SET (SOURCES
  main_DelilahQT.cpp
  MainWindow.cpp
  PlotWidget.cpp
  SamsonConnect.cpp
  SamsonQueryWidget.cpp
  QueueWidget.cpp
  StreamOperationWidget.cpp
  SamsonNodeWidget.cpp
  DelilahConnection.cpp
  Plot.cpp
  UpdateElements.cpp
  WorkerDataController.cpp
)

SET( HEADERS
	 Plot.h
     main_DelilahQT.h
     DelilahConnection.h
	 UpdateElements.h
	 WorkerDataController.h
)

SET (HEADERS_FOR_MOC
  MainWindow.h
  PlotWidget.h
  SamsonConnect.h
  SamsonQueryWidget.h
  QueueWidget.h
  StreamOperationWidget.h
  SamsonNodeWidget.h
)

SET(FORMS 
   MainWindow.ui
   SamsonConnect.ui
   SamsonQueryWidget.ui
   QueueWidget.ui
   StreamOperationWidget.ui
   SamsonNodeWidget.ui
)

SET(RESOURCES
#images.qrc
)

FIND_PACKAGE(Qt4 REQUIRED)

SET (STATIC_LIBS
     delilahLib
     pa
     lm
     protobuf
     pthread
     dl
     samsonNetwork
     samsonStream
     samsonModule
     engine
     au
	 tables
     pugi
     samsonCommon
     QtSvg
     QtXml
     QtCore
     kdchart
     ${COMMON_SAMSON_LIBS}
)


include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("/usr/local/KDAB/KDChart-2.3.0/src")
include_directories("/usr/local/Trolltech/Qt-4.7.4/include")
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})		# Required for QT intermediate files to compile

# QT Operations
# ------------------------------------------------------------
QT4_WRAP_CPP( HEADERS_MOC ${HEADERS_FOR_MOC} )
QT4_WRAP_UI(FORMS_HEADERS ${FORMS})
QT4_ADD_RESOURCES(RESOURCES_RCC ${RESOURCES})
# ------------------------------------------------------------

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})

# SAMSON Executable
ADD_EXECUTABLE( delilahQT ${SOURCES} ${HEADERS} ${HEADERS_FOR_MOC} ${HEADERS_MOC} ${FORMS_HEADERS} ${RESOURCES_RCC} )
TARGET_LINK_LIBRARIES(delilahQT ${STATIC_LIBS} )
TARGET_LINK_LIBRARIES( delilahQT ${QT_LIBRARIES} )

INSTALL(TARGETS delilahQT  RUNTIME DESTINATION   bin)



