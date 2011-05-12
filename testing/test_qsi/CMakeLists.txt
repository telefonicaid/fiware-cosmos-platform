find_package(Qt4 COMPONENTS QtCore QtGui QtSvg REQUIRED )
INCLUDE(${QT_USE_FILE})

# Include directories
# -----------------------------------------------------------------
include_directories("/usr/include/qt4")
include_directories("/usr/include/qt4/QtGui")
include_directories("${PROJECT_SOURCE_DIR}/libs/parseArgs")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/ui")



# Exec file
# -----------------------------------------------------------------
ADD_EXECUTABLE(test_qsi QsiTest.cpp )
TARGET_LINK_LIBRARIES(test_qsi lm pa qsi ${QT_LIBRARIES})



# TEST
# -----------------------------------------------------------------
# ADD_TEST(test_qsi ${PROJECT_SOURCE_DIR}/scripts/test_qsi )
