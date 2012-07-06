SET (HEADERS
Value.h
Filter.h
KeyValue.h
Source.h
SourceFunction.h
ValueReduce.h
smaz.h
ValueContainer.h
ValueVector.h
ProcessComponent.h
TopProcessComponent.h
ProcessComponentManager.h
HubMapProcess.h
HubMapTimeProcess.h
HitCountProcess.h
)

SET (SOURCES
Value.cpp
Filter.cpp
KeyValue.cpp
Source.cpp
SourceFunction.cpp
ValueReduce.cpp
smaz.cpp
ValueContainer.cpp
ValueVector.cpp
ProcessComponent.cpp
TopProcessComponent.cpp
ProcessComponentManager.cpp
HubMapProcess.cpp
HubMapTimeProcess.cpp
HitCountProcess.cpp
)

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")
include_directories("/usr/local/include/json")
include_directories("/usr/include/json")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samson_system STATIC ${SOURCES} ${HEADERS})
INSTALL(TARGETS samson_system ARCHIVE DESTINATION lib)
INSTALL(FILES ${HEADERS} DESTINATION include/samson_system)

SET (LIBSAMSONSYSTEM ${PROJECT_BINARY_DIR}/libs/samson_system/libsamson_system.a CACHE INTERNAL "")
