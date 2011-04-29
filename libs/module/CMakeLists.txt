SET (HEADERS_INSTALL
	samson/samson.h
	samson/Data.h
	samson/DataInstance.h
	samson/Factory.h
	samson/KV.h
	samson/KVFormat.h
	samson/KVSetStruct.h
	samson/KVVector.h
	samson/KVWriter.h
	samson/Module.h
	samson/Tracer.h
	samson/OperationController.h
	samson/Operation.h
	samson/Environment.h
	samson/samsonModuleHeader.h
	samson/samsonTypes.h
	samson/var_int.h
	samson/samsonVersion.h 
	samson/Log.h
)

SET (HEADERS
	ModulesManager.h
)

SET (SOURCES
    ModulesManager.cpp
)

# Inclue directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")
#include_directories("${PROJECT_SOURCE_DIR}/libs/network")
#include_directories("${PROJECT_SOURCE_DIR}/libs/memory")
include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
#include_directories("${PROJECT_BINARY_DIR}/libs/network")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")
#include_directories("${PROJECT_SOURCE_DIR}/libs/coding")
#include_directories("${PROJECT_SOURCE_DIR}/libs/disk")
#include_directories("${PROJECT_SOURCE_DIR}/libs/data")
#include_directories("${PROJECT_SOURCE_DIR}/libs/process")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonModule STATIC ${SOURCES} ${HEADERS} ${HEADERS_INSTALL} )


# Instalation header files
# -----------------------------------------------------------------
INSTALL(FILES ${HEADERS_INSTALL} DESTINATION include/samson)
