SET (SOURCES
AUTockenizer.cpp
DataCreator.cpp
DataType.cpp
DataContainer.cpp
ModuleContainer.cpp
OperationContainer.cpp
)

SET (HEADERS
DataContainer.h
AUTockenizer.h
DataCreator.h
DataType.h
ModuleContainer.h
OperationContainer.h
)

SET (LIBS
samsonCommon
protobuf
pthread
dl
au
lm
au
pa
${COMMON_SAMSON_LIBS}
)

# Include directories
# ------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Executables
# ------------------------------------------------------------

ADD_EXECUTABLE(samsonModuleParser main_samsonModuleParser.cpp  ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonModuleParser ${LIBS} )

ADD_EXECUTABLE(samsonModuleEditor main_samsonModuleEditor.cpp  ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(samsonModuleEditor ${LIBS} )


INSTALL(TARGETS               samsonModuleParser
        RUNTIME DESTINATION   bin)

INSTALL(TARGETS               samsonModuleEditor
        RUNTIME DESTINATION   bin)

# Build location of samsonModuleParser
SET (SAMSONMODULEPARSER ${PROJECT_BINARY_DIR}/apps/samsonModuleParser/samsonModuleParser CACHE INTERNAL "")
