SET (SOURCES
	main_samsonTopicMonitor.cpp
	MainWindow.cpp
)

SET( HEADERS
	 common.h
)

SET (HEADERS_FOR_MOC
  MainWindow.h
)

SET(FORMS 
   MainWindow.ui
)

SET(RESOURCES
#images.qrc
)

FIND_PACKAGE(Qt4 REQUIRED)

SET (STATIC_LIBS
  	 samsonClient
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


execute_process(COMMAND            echo ${QT_LIBRARIES})

# SAMSON Executable
ADD_EXECUTABLE( samsonTopicMonitor ${SOURCES} ${HEADERS} ${HEADERS_FOR_MOC} ${HEADERS_MOC} ${FORMS_HEADERS} ${RESOURCES_RCC} )
TARGET_LINK_LIBRARIES(samsonTopicMonitor ${STATIC_LIBS} )
TARGET_LINK_LIBRARIES( samsonTopicMonitor ${QT_LIBRARIES} ${QT_QTWEBKIT_LIBRARY} )

INSTALL(TARGETS samsonTopicMonitor  RUNTIME DESTINATION   bin)



