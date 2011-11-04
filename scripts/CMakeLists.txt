SET (FILES
    samsonProcessesSupervise
    samsonValgrindSupervise
	samsonModuleBootstrap
    samsonRun
)

# Instalation header files
# -----------------------------------------------------------------
INSTALL(PROGRAMS ${FILES} DESTINATION bin)
