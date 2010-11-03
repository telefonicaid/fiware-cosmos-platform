SET (SOURCES
AUTockenizer.cpp
DataContainer.cpp
DataCreator.cpp
DataType.cpp
ModuleContainer.cpp
OperationContainer.cpp
samson_module.cpp
)

SET (HEADERS
AUTockenizer.h
DataContainer.h
DataCreator.h
DataType.h
ModuleContainer.h
OperationContainer.h
samson_module.h
)


include_directories("${PROJECT_SOURCE_DIR}/libs/module")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")

link_directories("${PROJECT_BINARY_DIR}/libs/logMsg")

# SAMSON Executable
ADD_EXECUTABLE(samsonModuleParser ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonModuleParser samsonCommonStatic  protobuf pthread dl curses)

INSTALL(TARGETS               samsonModuleParser
        RUNTIME DESTINATION   bin)
