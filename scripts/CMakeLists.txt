SET (FILES
    samsonProcessesSupervise
    samsonValgrindSupervise
    samsonModuleBootstrap
    samsonPrepareServer
    samsonRun
    samsonModuleDist
    samsonInitSetup
)

# Instalation header files
# -----------------------------------------------------------------
INSTALL(PROGRAMS ${FILES} DESTINATION bin)
