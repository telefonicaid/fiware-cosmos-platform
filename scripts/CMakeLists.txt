SET (FILES
    samsonProcessesSupervise
	samsonValgrindSupervise
)

# Instalation header files
# -----------------------------------------------------------------
INSTALL(PROGRAMS ${FILES} DESTINATION bin)
