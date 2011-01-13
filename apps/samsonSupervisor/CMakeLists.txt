find_package(Qt4 COMPONENTS QtCore QtGui QtSvg REQUIRED )
INCLUDE(${QT_USE_FILE})

SET (SOURCES
    main_samsonSupervisor.cpp
	baTerm.cpp
	Process.cpp
	Spawner.cpp
	qt.cpp
	ButtonActions.cpp
	actions.cpp
	Starter.cpp
)

SET (HEADERS_MOC
	ButtonActions.h
	Starter.h
)

SET (HEADERS
	baTerm.h
	Process.h
	Spawner.h
	qt.h
	actions.h
	globals.h
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
