/* ****************************************************************************
*
* FILE                     platformProcesses.cpp - get/set platform process list
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Mar 02 2011
*
*/
#include <unistd.h>             // read
#include <fcntl.h>              // open, O_RDONLY, ...
#include <sys/stat.h>           // struct stat

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Lmt*
#include "samson/common/samsonDirectories.h"  // SAMSON_PLATFORM_PROCESSES
#include "samson/common/Process.h"            // Process
#include "samson/common/platformProcesses.h"  // Own interface
#include <stdlib.h>             // free()


namespace samson
{



/* ****************************************************************************
*
* ppFile - path to samson platform processes file
*/
static char* ppFile = (char*)  SAMSON_PLATFORM_PROCESSES;



/* ****************************************************************************
*
* platformProcessesSave - 
*/
void platformProcessesSave(ProcessVector* pvP)
{
	int    fd;
	char*  buf;
	int    tot;
	int    nb;
	int    size = sizeof(ProcessVector) + pvP->processes * sizeof(pvP->processV[0]);

	LM_T(LmtProcessVector, ("Saving Process Vector of %d processes", pvP->processes));
	LM_T(LmtProcessVector, ("sizeof(Process): %d", sizeof(pvP->processV[0])));
	LM_T(LmtProcessVector, ("file size should be: %d + %d * %d == %d",
		  sizeof(ProcessVector),
		  pvP->processes,
		  sizeof(pvP->processV[0]),
		  sizeof(ProcessVector) + pvP->processes * sizeof(pvP->processV[0])));

	if ((fd = open(ppFile, O_WRONLY | O_CREAT | O_TRUNC, 0744)) == -1)
		LM_RVE(("open-for-writing(%s): %s", ppFile, strerror(errno)));
	if (chmod(ppFile, 0744) != 0)
		LM_E(("Error setting permissions on '%s': %s", ppFile, strerror(errno)));

	buf  = (char*) pvP;
	tot  = 0;

	while (tot < size)
	{
		nb = write(fd, &buf[tot], size - tot);
		if (nb == -1)
		{
			close(fd);
			unlink(ppFile);
			LM_RVE(("write(%d bytes to '%s'): %s", size - tot, ppFile, strerror(errno)));
		}
		else if (nb == 0)
		{
			close(fd);
			unlink(ppFile);
			LM_RVE(("write(ZERO bytes to '%s'): %s", ppFile, strerror(errno)));
		}

		tot += nb;
	}

	close(fd);
	
	if (chmod(ppFile, 0744) != 0)
		LM_E(("chmod(%s): %s", ppFile, strerror(errno)));
}



/* ****************************************************************************
*
* platformProcessesGet -
*/
ProcessVector* platformProcessesGet(int* sizeP)
{
	int             s            = 0;
	bool            seriousError = true;
	struct stat     statBuf;
	char*           buf;
	int             fileSize;
	int             tot;
	int             nb;
	int             fd;
	ProcessVector*  pv;
	int             pvSize;

	LM_T(LmtProcessVector, ("Retrieving Process Vector"));

	if ((fd = open(ppFile, O_RDONLY)) == -1)
		return NULL;
	else if ((s = stat(ppFile, &statBuf)) == -1)
	{
		LM_E(("stat(%s): %s", ppFile, strerror(errno)));
		seriousError = true;
	}

	if (chmod(ppFile, 0744) != 0)
	{
		seriousError = true;
        if (errno != ENOENT)
			LM_E(("Error setting permissions on '%s': %s", ppFile, strerror(errno)));
		else
			seriousError = false;
	}

	if ((s == -1) || (fd == -1) || (statBuf.st_size == 0))
	{
		if (fd != -1)
			close(fd);

		LM_E(("s==%d, fd==%d, statBuf.st_size==%d", s, fd, statBuf.st_size));
		LM_RE(NULL, ("problems with samson platform processes file '%s'", ppFile));
	}
	else
	{
		LM_T(LmtProcessVector, ("Retrieving process vec data from file '%s'", ppFile));

		fileSize = statBuf.st_size;
		buf      = (char*) calloc(1, fileSize);

		if (buf == NULL)
			LM_RE(NULL, ("error allocating room for Process Vector (%s bytes): %s", fileSize, strerror(errno)));
		LM_T(LmtInit, ("Allocated a buffer of %d bytes for the process vector", fileSize));

		tot = 0;
		while (tot < fileSize)
		{
			nb = read(fd, &buf[tot], fileSize - tot);
			if (nb == -1)
			{
				close(fd);
				LM_RE(NULL, ("Error reading from process vector file '%s': %s", ppFile, strerror(errno)));
			}
			else if (nb == 0)
			{
				close(fd);
				LM_RE(NULL, ("Error reading from process vector file '%s'", ppFile));
			}

			tot += nb;
		}

		close(fd);

		pv      = (ProcessVector*) buf;
		pvSize  = sizeof(ProcessVector) + pv->processes * sizeof(Process);

		LM_T(LmtProcessVector, ("file size: %d", fileSize));
		LM_T(LmtProcessVector, ("%d processes, each of a size of %d => %d + %d * %d == %d",
			  pv->processes,
			  sizeof(pv->processV[0]),
			  sizeof(ProcessVector),
			  pv->processes,
			  sizeof(pv->processV[0]),
			  sizeof(ProcessVector) + pv->processes * sizeof(pv->processV[0])));

		if (pvSize != fileSize)
			LM_RE(NULL, ("Size of file '%s' (%d bytes) does not match the size of a Process Vector of %d processes: %d bytes", ppFile, fileSize, pv->processes, pvSize));

		LM_T(LmtInit, ("Read Processes from '%s' - got %d processes", ppFile, pv->processes));
		close(fd);

		LM_T(LmtProcessVector, ("Got %d processes", pv->processes));
		for (int ix = 0; ix < pv->processes; ix++)
		{
			Process* processP = &pv->processV[ix];

			LM_T(LmtProcessVector, ("  process %02d: alias: '%s', name: '%s', host: '%s'. port: %d", ix, processP->alias, processP->name, processP->host, processP->port));
		}
	}

	pv->processVecSize = pvSize;

	if (sizeP != NULL)
		*sizeP = pvSize;

	return pv;
}

}
