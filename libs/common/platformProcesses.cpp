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

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Lmt*
#include "samsonDirectories.h"  // SAMSON_PLATFORM_PROCESSES
#include "Worker.h"             // WorkerVectorData - will be replaced
#include "Process.h"            // Process - that will replace WorkerVectorData
#include "platformProcesses.h"  // Own interface
#include <stdlib.h>             // free()


namespace ss
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
void platformProcessesSave(WorkerVectorData* wvP)
{
	int    fd;
	char*  buf;
	int    tot;
	int    nb;
	int    size = sizeof(WorkerVectorData) + wvP->workers * sizeof(wvP->workerV[0]);

	LM_T(LmtWorkerVector, ("Saving Worker Vector of %d workers", wvP->workers));
	LM_T(LmtWorkerVector, ("sizeof(Worker): %d", sizeof(wvP->workerV[0])));
	LM_T(LmtWorkerVector, ("file size should be: %d + %d * %d == %d",
		  sizeof(WorkerVectorData),
		  wvP->workers,
		  sizeof(wvP->workerV[0]),
		  sizeof(WorkerVectorData) + wvP->workers * sizeof(wvP->workerV[0])));

	if ((fd = open(ppFile, O_WRONLY | O_CREAT | O_TRUNC, 0744)) == -1)
		LM_RVE(("open-for-writing(%s): %s", ppFile, strerror(errno)));
	if (chmod(ppFile, 0744) != 0)
		LM_E(("Error setting permissions on '%s': %s", ppFile, strerror(errno)));

	buf  = (char*) wvP;
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
WorkerVectorData* platformProcessesGet(int* sizeP)
{
	int                s            = 0;
	bool               seriousError = true;
	struct stat        statBuf;
	char*              buf;
	int                fileSize;
	int                tot;
	int                nb;
	int                fd;
	WorkerVectorData*  wv;
	int                wvSize;

	LM_T(LmtWorkerVector, ("Retrieving Worker Vector"));

	if ((fd = open(ppFile, O_RDONLY)) == -1)
		LM_RE(NULL, ("error opening samson platform processes file '%s': %s", ppFile, strerror(errno)));
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
		LM_RE(NULL, ("problems with samson platform processes file '%s'", ppFile));
	else
	{
		LM_T(LmtWorkerVector, ("Retrieving worker vec data from file '%s'", ppFile));

		fileSize = statBuf.st_size;
		buf      = (char*) calloc(1, fileSize);

		if (buf == NULL)
			LM_RE(NULL, ("error allocating room for Worker Vector (%s bytes): %s", fileSize, strerror(errno)));
		LM_T(LmtInit, ("Allocated a buffer of %d bytes for the worker vector", fileSize));

		tot = 0;
		while (tot < fileSize)
		{
			nb = read(fd, &buf[tot], fileSize - tot);
			if (nb == -1)
				LM_RE(NULL, ("Error reading from worker vector file '%s': %s", ppFile, strerror(errno)));
			else if (nb == 0)
				LM_RE(NULL, ("Error reading from worker vector file '%s'", ppFile));

			tot += nb;
		}

		wv      = (WorkerVectorData*) buf;
		wvSize  = sizeof(WorkerVectorData) + wv->workers * sizeof(Worker);

		LM_T(LmtWorkerVector, ("file size: %d", fileSize));
		LM_T(LmtWorkerVector, ("%d workers, each of a size of %d => %d + %d * %d == %d",
			  wv->workers,
			  sizeof(wv->workerV[0]),
			  sizeof(WorkerVectorData),
			  wv->workers,
			  sizeof(wv->workerV[0]),
			  sizeof(WorkerVectorData) + wv->workers * sizeof(wv->workerV[0])));

		if (wvSize != fileSize)
			LM_RE(NULL, ("Size of file '%s' (%d bytes) does not match the size of a Worker Vector of %d workers: %d bytes", ppFile, fileSize, wv->workers, wvSize));

		LM_T(LmtInit, ("Read Workers from '%s' - got %d workers", ppFile, wv->workers));
		close(fd);

		LM_T(LmtWorkerVector, ("Got %d workers", wv->workers));
		for (int ix = 0; ix < wv->workers; ix++)
		{
			Worker* workerP = &wv->workerV[ix];

			LM_T(LmtWorkerVector, ("  worker %02d: alias: '%s', name: '%s', host: '%s'. port: %d", ix, workerP->alias, workerP->name, workerP->ip, workerP->port));
		}
	}

	*sizeP = wvSize;

	return wv;
}

}
