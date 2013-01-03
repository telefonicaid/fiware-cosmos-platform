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
include_directories("${PROJECT_SOURCE_DIR}/libs/extra")


# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(samson_comscore STATIC ${SOURCES} ${HEADERS})
INSTALL(TARGETS samson_comscore ARCHIVE DESTINATION lib)
INSTALL(FILES ${HEADERS} DESTINATION include/comscore)

SET( STATIC_LIBS
     samson_comscore
     au
     lm
	 json
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

ADD_EXECUTABLE( samson_comscore_directory_cat main_samson_comscore_directory_cat.cpp )
TARGET_LINK_LIBRARIES(samson_comscore_directory_cat ${STATIC_LIBS})


INSTALL(TARGETS               samson_comscore_directory_create
        RUNTIME DESTINATION   bin)

INSTALL(TARGETS               samson_comscore_directory_query
        RUNTIME DESTINATION   bin)

INSTALL(TARGETS               samson_comscore_directory_cat
        RUNTIME DESTINATION   bin)


