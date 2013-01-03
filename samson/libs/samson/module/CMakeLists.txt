SET (HEADERS_INSTALL
	samson.h
	Data.h
	DataInstance.h
	Factory.h
	KV.h
	KVFormat.h
	KVSetStruct.h
	KVVector.h
	KVWriter.h
	Module.h
	Tracer.h
	Operation.h
	OperationController.h
	Environment.h
	samsonModuleHeader.h
	samsonTypes.h
	var_int.h
	samsonVersion.h 
	Log.h
	List.h
)

SET (HEADERS
	ModulesManager.h
	List.h
)

SET (SOURCES
    ModulesManager.cpp
	samson.cpp
)

SET (LIBRARIES 
   lm 
   au 
   samsonCommon 
   dl
)
   
# Inclue directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonModule SHARED ${SOURCES} ${HEADERS} ${HEADERS_INSTALL} )
TARGET_LINK_LIBRARIES(samsonModule ${LIBRARIES})

# Instalation header files
# -----------------------------------------------------------------
INSTALL(FILES ${HEADERS_INSTALL} DESTINATION include/samson/module )
INSTALL(TARGETS samsonModule  DESTINATION   lib)
