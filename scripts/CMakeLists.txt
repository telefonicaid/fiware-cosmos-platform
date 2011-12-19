SET (FILES
    samsonProcessesSupervise
    samsonValgrindSupervise
	samsonModuleBootstrap
	samsonPrepareServer
    samsonRun
)

# Instalation header files
# -----------------------------------------------------------------
INSTALL(PROGRAMS ${FILES} DESTINATION bin)
