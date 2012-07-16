SET (FILES
    samsonProcessesSupervise
    samsonValgrindSupervise
    samsonModuleBootstrap
    samsonPrepareServer
    samsonModuleDist
    samsonInitSetup
)

# Instalation header files
# -----------------------------------------------------------------
INSTALL(PROGRAMS ${FILES} DESTINATION bin)
