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
	samson/Operation.h
	samson/Environment.h
	samson/samsonModuleHeader.h
	samson/samsonTypes.h
	samson/var_int.h
	samson/samsonVersion.h 
	samson/Log.h
)


SET( HEADERS_NO_INSTALL
     EnvironmentOperations.h
)


SET (SOURCES
	main.cpp
     EnvironmentOperations.cpp
)


include_directories("${PROJECT_SOURCE_DIR}/libs/logMsg")
include_directories("${PROJECT_SOURCE_DIR}/libs/common")


ADD_LIBRARY(samsonModuleStatic STATIC ${SOURCES} ${HEADERS} ${HEADERS_NO_INSTALL} )
#ADD_LIBRARY(samsonModuleShared SHARED ${SOURCES} ${HEADERS} )

TARGET_LINK_LIBRARIES( samsonModuleStatic protobuf )

SET_TARGET_PROPERTIES( samsonModuleStatic PROPERTIES OUTPUT_NAME samsonModule)

INSTALL(FILES ${HEADERS} DESTINATION include/samson)
