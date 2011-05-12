find_package(Qt4 COMPONENTS QtCore QtGui QtSvg REQUIRED )
INCLUDE(${QT_USE_FILE})

SET (HEADERS
	QsiBase.h
	QsiBlock.h
	QsiCallback.h
	QsiFunction.h
)


SET (HEADERS_MOC
	QsiManager.h
)

SET (SOURCES
	QsiBlock.cpp
	QsiManager.cpp
)



QT4_WRAP_CPP(MOC_SOURCES ${HEADERS_MOC})

# Include directories
# -----------------------------------------------------------------
include_directories("/usr/include/qt4")
include_directories("/usr/include/qt4/QtGui")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")



# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(qsi STATIC ${SOURCES} ${MOC_SOURCES} ${HEADERS})
