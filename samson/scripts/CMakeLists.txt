SET (FILES
    samsonProcessesSupervise
    samsonValgrindSupervise
    samsonModuleBootstrap
    samsonPrepareServer
    samsonModuleDist
    samsonInitSetup
	samsonTest
)

# Instalation header files
# -----------------------------------------------------------------
INSTALL(PROGRAMS ${FILES} DESTINATION bin)
