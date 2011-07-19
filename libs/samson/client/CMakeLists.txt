
SET (HEADERS
	SamsonClient.h
)

SET (SOURCES
	SamsonClient.cpp
)

SET (LIBS
    engine
    lm
    samsonCommon
    samsonNetwork
    samsonModule
    delilahLib
)
	
	
# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonClient ${SOURCES} ${HEADERS})

TARGET_LINK_LIBRARIES(samsonClient ${LIBS} )


INSTALL(TARGETS samsonClient  DESTINATION   lib)
