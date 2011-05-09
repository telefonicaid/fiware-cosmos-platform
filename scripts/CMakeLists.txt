SET (FILES
    samsonProcessesSupervise
	samsonValgrindSupervise
	samsonRun
)

# Instalation header files
# -----------------------------------------------------------------
INSTALL(PROGRAMS ${FILES} DESTINATION bin)
