/* ****************************************************************************
*
* FILE                     main_samsonController.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <unistd.h>             // read
#include <fcntl.h>              // open, O_RDONLY, ...
#include <sys/stat.h>           // struct stat

#include "parseArgs.h"          // parseArgs

#include "ports.h"              // CONTROLLER_PORT
#include "samsonDirectories.h"  // SAMSON_PLATFORM_PROCESSES
#include "SamsonController.h"	// ss::SamsonController
#include "SamsonSetup.h"		// ss::SamsonSetup
#include "platformProcesses.h"  // ss::platformProcessesGet, ss::platformProcessesSave
#include "MemoryManager.h"		// ss::MemoryManager
#include "DiskManager.h"		// ss::DiskManager
#include "FileManager.h"		// ss::FileManager
#include "LockDebugger.h"       // au::LockDebugger



/* ****************************************************************************
*
* Option variables
*/
int              endpoints;
char			 workingDir[1024];



#define DEF_WD   _i SAMSON_DEFAULT_WORKING_DIRECTORY
#define DEF_WF   _i SAMSON_PLATFORM_PROCESSES
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-working",     workingDir,    "WORKING",                  PaString, PaOpt, DEF_WD,  PaNL,  PaNL, "working directory"       },
	{ "-endpoints",  &endpoints,     "ENDPOINTS",                PaInt,    PaOpt,     80,     3,   100, "number of endpoints"     },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int logFd = -1;


#if 0
/* ****************************************************************************
*
* platformProcessesSave - 
*/
void platformProcessesSave(ss::WorkerVectorData* wvP)
{
	int    fd;
	char*  buf;
	int    tot;
	int    nb;
	int    size = sizeof(ss::WorkerVectorData) + wvP->workers * sizeof(wvP->workerV[0]);

	LM_T(LmtWorkerVector, ("Saving Worker Vector of %d workers", wvP->workers));
	LM_T(LmtWorkerVector, ("sizeof(Worker): %d", sizeof(wvP->workerV[0])));
	LM_T(LmtWorkerVector, ("file size should be: %d + %d * %d == %d",
		  sizeof(ss::WorkerVectorData),
		  wvP->workers,
		  sizeof(wvP->workerV[0]),
		  sizeof(ss::WorkerVectorData) + wvP->workers * sizeof(wvP->workerV[0])));

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
* processVecGet - 
*/
static ss::WorkerVectorData* processVecGet(int* sizeP)
{
	int                             s            = 0;
	bool                            seriousError = true;
	struct stat                     statBuf;
	char*                           buf;
	int                             fileSize;
	int                             tot;
	int                             nb;
	int                             fd;
	ss::WorkerVectorData*  wv;
	int                             wvSize;

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

		wv      = (ss::WorkerVectorData*) buf;
		wvSize  = sizeof(ss::WorkerVectorData) + wv->workers * sizeof(ss::Worker);

		LM_T(LmtWorkerVector, ("file size: %d", fileSize));
		LM_T(LmtWorkerVector, ("%d workers, each of a size of %d => %d + %d * %d == %d",
			  wv->workers,
			  sizeof(wv->workerV[0]),
			  sizeof(ss::WorkerVectorData),
			  wv->workers,
			  sizeof(wv->workerV[0]),
			  sizeof(ss::WorkerVectorData) + wv->workers * sizeof(wv->workerV[0])));

		if (wvSize != fileSize)
			LM_RE(NULL, ("Size of file '%s' (%d bytes) does not match the size of a Worker Vector of %d workers: %d bytes", ppFile, fileSize, wv->workers, wvSize));

		LM_T(LmtInit, ("Read Workers from '%s' - got %d workers", ppFile, wv->workers));
		close(fd);

		LM_T(LmtWorkerVector, ("Got %d workers", wv->workers));
		for (int ix = 0; ix < wv->workers; ix++)
		{
			ss::Worker* workerP = &wv->workerV[ix];

			LM_T(LmtWorkerVector, ("  worker %02d: alias: '%s', name: '%s', host: '%s'. port: %d", ix, workerP->alias, workerP->name, workerP->ip, workerP->port));
		}
	}

	*sizeP = wvSize;

	return wv;
}
#endif



/* ****************************************************************************
*
* main - main routine for the samsonController
*/
int main(int argC, const char* argV[])
{
	ss::ProcessVector*  processVec;
	int                 processVecSize;

	paConfig("prefix",                        (void*) "SSC_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_T(LmtInit, ("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

	processVec = ss::platformProcessesGet(&processVecSize);
	if (processVec == NULL)
		LM_X(1, ("Error retreiving info about the platform processes - can't function without it!"));

	LM_T(LmtInit, ("%d workers", processVec->processes));

	au::LockDebugger::shared();         // Lock usage debugging (necessary here where there is only one thread)
	ss::SamsonSetup::load(workingDir);  // Load setup and create all directories
	ss::DiskManager::shared();          // Disk manager
	ss::MemoryManager::init();          // Memory manager
	ss::ModulesManager::init();			// Init the modules manager
	
	
	
	// Instance of network object and initialization
	// ---------------------------------------------
	ss::Network network(ss::Endpoint::Controller, "Controller", CONTROLLER_PORT, endpoints, processVec->processes);

	network.initAsSamsonController();
	network.workerVecSet(processVec, processVecSize, ss::platformProcessesSave);
	network.runInBackground();
	
	
	// Instance of the Samson Controller
	// ---------------------------------
	
	ss::SamsonController  controller(&network);

	controller.runBackgroundProcesses();
	controller.touch();
	
	while (true)
		sleep(10000);
}
