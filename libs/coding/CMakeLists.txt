SET (HEADERS
    coding.h
)

SET (SOURCES
    coding.cpp
)

include_directories("${PROJECT_SOURCE_DIR}/libs/common")
include_directories("${PROJECT_SOURCE_DIR}/libs/module")

ADD_LIBRARY(samsonCodingStatic STATIC ${SOURCES} ${HEADERS} )
ADD_LIBRARY(samsonCodingShared SHARED ${SOURCES} ${HEADERS} )

SET_TARGET_PROPERTIES(samsonCodingShared PROPERTIES OUTPUT_NAME samsonCoding)
SET_TARGET_PROPERTIES(samsonCodingStatic PROPERTIES OUTPUT_NAME samsonCoding)



