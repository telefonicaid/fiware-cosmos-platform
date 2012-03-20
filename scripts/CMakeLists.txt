SET (FILES
    samsonProcessesSupervise
    samsonValgrindSupervise
    samsonModuleBootstrap
    samsonPrepareServer
    samsonRun
    samsonModuleDist
    samsonInitSetup
	samsonAutomaticTest
)

# Instalation header files
# -----------------------------------------------------------------
INSTALL(PROGRAMS ${FILES} DESTINATION bin)
