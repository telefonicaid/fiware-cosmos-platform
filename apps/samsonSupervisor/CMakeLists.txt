find_package(Qt4 COMPONENTS QtCore QtGui QtSvg REQUIRED )
INCLUDE(${QT_USE_FILE})

SET (SOURCES
    main_samsonSupervisor.cpp
	SamsonSupervisor.cpp
	baTerm.cpp
	processList.cpp
	spawnerList.cpp
	Starter.cpp
	starterList.cpp
	qt.cpp
	actions.cpp
	TabManager.cpp
	DelilahTab.cpp
	LogTab.cpp
	ProcessListTab.cpp
	configFile.cpp
)

SET (HEADERS_MOC
	Starter.h
	TabManager.h
	DelilahTab.h
	LogTab.h
	ProcessListTab.h
)

SET (HEADERS
	SamsonSupervisor.h
	baTerm.h
	Process.h
	processList.cpp
	Spawner.h
	spawnerList.cpp
	starterList.cpp
	qt.h
	actions.h
	globals.h
	configFile.h
)


QT4_WRAP_CPP(MOC_SOURCES ${HEADERS_MOC})

include_directories("/usr/include/qt4")
include_directories("/usr/include/qt4/QtGui")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/network")
include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/memory")

link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")
link_directories("${PROJECT_BINARY_DIR}/libs/parseArgs")

# SAMSON Executable
ADD_EXECUTABLE(samsonSupervisor ${SOURCES} ${MOC_SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonSupervisor samsonNetworkStatic pa lm pthread ${QT_LIBRARIES})

INSTALL(TARGETS               samsonSupervisor
        RUNTIME DESTINATION   bin)
