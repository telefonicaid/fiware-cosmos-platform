find_package(Qt4 COMPONENTS QtCore QtGui QtSvg REQUIRED )
INCLUDE(${QT_USE_FILE})

SET (HEADERS
	QsiAlignment.h
	QsiBase.h
	QsiBlock.h
	QsiBox.h
	QsiCallback.h
	QsiDialog.h
	QsiExpandList.h
	QsiFrame.h
	QsiFunction.h
	QsiInputDialog.h
	QsiInputLine.h
	QsiInputReturnFunction.h
	QsiManager.h
	traceLevels.h
)


SET (HEADERS_MOC
	QsiManager.h
)

SET (SOURCES
	QsiBase.cpp
	QsiBlock.cpp
	QsiBox.cpp
	QsiManager.cpp
	QsiFrame.cpp
	QsiExpandList.cpp
	QsiDialog.cpp
	QsiInputLine.cpp
	QsiInputDialog.cpp
)



QT4_WRAP_CPP(MOC_SOURCES ${HEADERS_MOC})

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("/usr/include/qt4")
include_directories("/usr/include/qt4/QtGui")




# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(qsi STATIC ${SOURCES} ${MOC_SOURCES} ${HEADERS})

