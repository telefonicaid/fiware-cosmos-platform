SET (SOURCES
    main_delilah_graph.cpp
)

SET (HEADERS
)

SET (STATIC_LIBS
     delilahQtLib
     delilahLib
     pa
     lm
     protobuf
     pthread
     curses
     samsonNetwork
     samsonStream
     samsonModule
     dl
     engine
     au
     samsonCommon	
     QtCore
     ${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})		# Required for QT intermediate files to compile


SET (HEADERS_FOR_MOC
	DelilahQt.h
)

FIND_PACKAGE(Qt4 REQUIRED)

# QT Operations
# ------------------------------------------------------------
#QT4_WRAP_CPP( HEADERS_MOC ${HEADERS_FOR_MOC} )
#QT4_WRAP_UI(FORMS_HEADERS ${FORMS})
#QT4_ADD_RESOURCES(RESOURCES_RCC ${RESOURCES})
# ------------------------------------------------------------

INCLUDE(${QT_USE_FILE})
ADD_DEFINITIONS(${QT_DEFINITIONS})


# Executable declaration
# ------------------------------------------------------------
ADD_EXECUTABLE(delilah_graph ${SOURCES} ${HEADERS} )
TARGET_LINK_LIBRARIES(delilah_graph ${STATIC_LIBS} )

INSTALL(TARGETS               delilah_graph
        RUNTIME DESTINATION   bin)
