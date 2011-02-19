SET (HEADERS
	logMsg.h
	traceLevels.h
)

SET (SOURCES
    logMsg.cpp
    traceLevels.cpp
)

ADD_LIBRARY(logMsgStatic STATIC ${SOURCES} ${HEADERS} )
#ADD_LIBRARY(logMsgShared SHARED ${SOURCES} ${HEADERS} )

TARGET_LINK_LIBRARIES(logMsgStatic)
#TARGET_LINK_LIBRARIES( logMsgShared )


SET_TARGET_PROPERTIES(logMsgStatic PROPERTIES OUTPUT_NAME lm)
#SET_TARGET_PROPERTIES(logMsgShared PROPERTIES OUTPUT_NAME lm)

#
#install(TARGETS                logMsgShared
#        LIBRARY  DESTINATION   lib)
#
#INSTALL(FILES ${HEADERS} DESTINATION include)
