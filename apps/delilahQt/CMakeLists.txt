SET (SOURCES
  main_DelilahQT.cpp
  MainWindow.cpp
  SamsonConnect.cpp
  SamsonQueryWidget.cpp
  SamsonNodeWidget.cpp
  DelilahConnection.cpp
)

SET( HEADERS
  main_DelilahQT.h
  DelilahConnection.h
)

SET (HEADERS_FOR_MOC
  MainWindow.h
  SamsonConnect.h
  SamsonQueryWidget.h
  SamsonNodeWidget.h
)

SET(FORMS 
   MainWindow.ui
   SamsonConnect.ui
   SamsonQueryWidget.ui
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
     curses
     readline
     samsonNetwork
     samsonStream
     samsonModule
     engine
     au
     pugi
     samsonCommon
     ${COMMON_SAMSON_LIBS}
)


include_directories("${PROJECT_SOURCE_DIR}/libs")
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



