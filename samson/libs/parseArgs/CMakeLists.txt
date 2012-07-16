SET (HEADERS
	paBuiltin.h
	paConfig.h
	paDefaultValues.h
	paEnvVals.h
	paFrom.h
	paFullName.h
	paGetVal.h
	paIsOption.h
	paIsSet.h
	paIterate.h
	paIxLookup.h
	paLimitCheck.h
	paLog.h
	paLogSetup.h
	paOptions.h
	paOptionsParse.h
	paParse.h
	paPrivate.h
	paRcFile.h
	paTraceLevels.h
	paUsage.h
	paValueFrom.h
	paWarning.h
	parseArgs.h
	baStd.h
)

SET (SOURCES
	paBuiltin.cpp
	paConfig.cpp
	paDefaultValues.cpp
	paEnvVals.cpp
	paFrom.cpp
	paFullName.cpp
	paGetVal.cpp
	paIsOption.cpp
	paIsSet.cpp
	paIterate.cpp
	paIxLookup.cpp
	paLimitCheck.cpp
	paLogSetup.cpp
	paOptions.cpp
	paOptionsParse.cpp
	paParse.cpp
	paRcFile.cpp
	paUsage.cpp
	paValueFrom.cpp
	paWarning.cpp
	baStd.cpp
)

# Include directories
# -----------------------------------------------------------------
include_directories("${PROJECT_SOURCE_DIR}/libs")

# Library declaration
# -----------------------------------------------------------------
ADD_LIBRARY(pa STATIC ${SOURCES} ${HEADERS})
SET (LIBPA ${PROJECT_BINARY_DIR}/libs/parseArgs/libpa.a CACHE INTERNAL "")

# Only needed for the "samson" project
IF (CMAKE_PROJECT_NAME STREQUAL "samson")
    INSTALL(TARGETS pa ARCHIVE DESTINATION lib)
    INSTALL(FILES ${HEADERS} DESTINATION include/parseArgs)
ENDIF (CMAKE_PROJECT_NAME STREQUAL "samson")
