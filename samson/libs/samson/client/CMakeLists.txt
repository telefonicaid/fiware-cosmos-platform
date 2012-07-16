
SET (HEADERS
	SamsonClient.h
	SamsonPushBuffer.h
	BufferContainer.h
	SamsonClientBlock.h
	SamsonClientBlockInterface.h
)

SET (SOURCES
	SamsonClient.cpp
	SamsonPushBuffer.cpp
	BufferContainer.cpp
	SamsonClientBlock.cpp
)

	
# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samsonClient STATIC ${SOURCES} ${HEADERS})


INSTALL(TARGETS samsonClient  DESTINATION   lib)
