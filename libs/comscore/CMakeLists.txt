SET (HEADERS
	SamsonComscoreDictionary.h
	MapId2Id.h
	common.h
)

SET (SOURCES
	SamsonComscoreDictionary.cpp
	MapId2Id.cpp
	common.cpp
)


# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samson_comscore STATIC ${SOURCES} ${HEADERS})
INSTALL(TARGETS samson_comscore ARCHIVE DESTINATION lib)


SET( STATIC_LIBS
     tables
     samson_comscore
     au
     lm
     pa
     dl
     pthread
)


# Executable declaration
# ------------------------------------------------------------

ADD_EXECUTABLE( samson_comscore_directory_create main_samson_comscore_directory_create.cpp )
TARGET_LINK_LIBRARIES(samson_comscore_directory_create ${STATIC_LIBS})

ADD_EXECUTABLE( samson_comscore_directory_query  main_samson_comscore_directory_query.cpp )
TARGET_LINK_LIBRARIES(samson_comscore_directory_query ${STATIC_LIBS})

INSTALL(TARGETS               samson_comscore_directory_create
        RUNTIME DESTINATION   bin)

INSTALL(TARGETS               samson_comscore_directory_query
        RUNTIME DESTINATION   bin)