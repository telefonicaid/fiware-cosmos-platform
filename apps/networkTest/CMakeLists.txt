SET (SOURCES
    networkTest.cpp
)

SET (HEADERS
)


include_directories("${PROJECT_SOURCE_DIR}/libs/")

# networkTest Executable
ADD_EXECUTABLE(networkTest ${SOURCES} ${HEADERS})
TARGET_LINK_LIBRARIES(networkTest samsonNetworkStatic pa lm pthread ${COMMON_SAMSON_LIBS})

INSTALL(TARGETS               networkTest
        RUNTIME DESTINATION   bin)
