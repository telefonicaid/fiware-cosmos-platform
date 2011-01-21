find_package(Qt4 COMPONENTS QtCore QtGui QtSvg REQUIRED )
INCLUDE(${QT_USE_FILE})

SET (SOURCES
	main_samsonLogServer.cpp
	logProviderList.cpp
	LogProvider.cpp
	Popup.cpp
)

SET (HEADERS_MOC
	LogProvider.h
)

SET (HEADERS
	logProviderList.h
	traceLevels.h
	Popup.h
)

QT4_WRAP_CPP(MOC_SOURCES ${HEADERS_MOC})

include_directories("/usr/include/qt4")
include_directories("/usr/include/qt4/QtGui")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")


link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/parseArgs")

# SAMSON Executable
ADD_EXECUTABLE(samsonLogServer ${SOURCES} ${MOC_SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonLogServer samsonNetworkStatic samsonCommonStatic pa lm pthread ${QT_LIBRARIES})

INSTALL(TARGETS               samsonLogServer
        RUNTIME DESTINATION   bin)
