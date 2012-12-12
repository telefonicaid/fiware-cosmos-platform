SET (FILES
    samsonProcessesSupervise
    samsonValgrindSupervise
    samsonModuleBootstrap
    samsonPrepareServer
    samsonModuleDist
    samsonInitSetup
    samsonTest
    testDiff.py
)

# Instalation header files
# -----------------------------------------------------------------
INSTALL(PROGRAMS ${FILES} DESTINATION bin)
