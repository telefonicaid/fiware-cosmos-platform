SET (FILES
    samsonProcessesSupervise
    samsonValgrindSupervise
	samsonModuleBootstrap
	samsonPrepareServer
    samsonRun
    samsonModuleDist
)

# Instalation header files
# -----------------------------------------------------------------
INSTALL(PROGRAMS ${FILES} DESTINATION bin)
