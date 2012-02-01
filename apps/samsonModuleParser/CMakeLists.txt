SET (SOURCES
AUTockenizer.cpp
DataCreator.cpp
DataType.cpp
DataContainer.cpp
ModuleContainer.cpp
OperationContainer.cpp
samson_module.cpp
)

SET (HEADERS
DataContainer.h
AUTockenizer.h
DataCreator.h
DataType.h
ModuleContainer.h
OperationContainer.h
samson_module.h
)

SET (LIBS
samsonCommon
protobuf
pthread
dl
au
lm
pa
${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# SAMSON Executable
# ------------------------------------------------------------
ADD_EXECUTABLE(samsonModuleParser ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonModuleParser ${LIBS} )

INSTALL(TARGETS               samsonModuleParser
        RUNTIME DESTINATION   bin)

# Build location of samsonModuleParser
SET (SAMSONMODULEPARSER ${PROJECT_BINARY_DIR}/apps/samsonModuleParser/samsonModuleParser CACHE INTERNAL "")
