SET (HEADERS
	samson/samsonModuleHeader.h
	samson/samsonVersion.h
	samson/samsonTypes.h
	samson/DataInstance.h
	samson/Data.h
	samson/Operation.h
	samson/KVFormat.h
	samson/Factory.h
	samson/Module.h
	samson/samsonVersion.h
	samson/KV.h
	samson/KVVector.h
	samson/KVWriter.h
	samson/KVSetStruct.h
)

SET (SOURCES
    main.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/libs/common")

ADD_LIBRARY(samsonCommonStatic STATIC ${SOURCES} ${HEADERS})

SET_TARGET_PROPERTIES(samsonCommonStatic  PROPERTIES OUTPUT_NAME samsonCommon)

#TARGET_LINK_LIBRARIES(samsonCoreStatic samsonProtocolsStatic protobuf z curses pthread)

INSTALL(FILES ${HEADERS} DESTINATION include/samson)

