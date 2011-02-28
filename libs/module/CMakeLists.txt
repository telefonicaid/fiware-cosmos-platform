SET (HEADERS
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

# Instalation header files
# -----------------------------------------------------------------
INSTALL(FILES ${HEADERS} DESTINATION include/samson)
