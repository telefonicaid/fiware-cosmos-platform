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
#include "MemoryManager.h"		// ss::MemoryManager
#include "DiskManager.h"		// ss::DiskManager
#include "FileManager.h"		// ss::FileManager
#include "LockDebugger.h"       // au::LockDebugger
#include "Message.h"            // WorkerVectorData



/* ****************************************************************************
*
* Option variables
*/
int              endpoints;
char			 workingDir[1024];
char			 ppFile[1024];



#define DEF_WD   _i SAMSON_DEFAULT_WORKING_DIRECTORY
#define DEF_WF   _i SAMSON_PLATFORM_PROCESSES
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-working",     workingDir,    "WORKING",                  PaString, PaOpt, DEF_WD,  PaNL,  PaNL, "working directory"       },
	{ "-ppFile",      ppFile,        "PLATFORM_PROCESSES_FILE",  PaString, PaOpt, DEF_WF,  PaNL,  PaNL, "platform processes file" },
	{ "-endpoints",  &endpoints,     "ENDPOINTS",                PaInt,    PaOpt,     80,     3,   100, "number of endpoints"     },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int                             logFd      = -1;
ss::Message::WorkerVectorData*  workerVec  = NULL;
int                             workerVecSize;



/* ****************************************************************************
*
* workerVecSave - 
*/
void workerVecSave(void)
{
	int    fd;
	char*  buf;
	int    tot;
	int    nb;

	LM_T(LmtWorkerVector, ("Saving Worker Vector of %d workers", workerVec->workers));
	LM_T(LmtWorkerVector, ("sizeof(Worker): %d", sizeof(workerVec->workerV[0])));
	LM_T(LmtWorkerVector, ("file size should be: %d + %d * %d == %d",
		  sizeof(ss::Message::WorkerVectorData),
		  workerVec->workers,
		  sizeof(workerVec->workerV[0]),
		  sizeof(ss::Message::WorkerVectorData) + workerVec->workers * sizeof(workerVec->workerV[0])));

	if ((fd = open(ppFile, O_WRONLY | O_CREAT | O_TRUNC, 0744)) == -1)
		LM_RVE(("open-for-writing(%s): %s", ppFile, strerror(errno)));
	if (chmod(ppFile, 0744) != 0)
		LM_E(("Error setting permissions on '%s': %s", ppFile, strerror(errno)));

	buf  = (char*) workerVec;
	tot  = 0;

	while (tot < workerVecSize)
	{
		nb = write(fd, &buf[tot], workerVecSize - tot);
		if (nb == -1)
		{
			close(fd);
			unlink(ppFile);
			LM_RVE(("write(%d bytes to '%s'): %s", workerVecSize - tot, ppFile, strerror(errno)));
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
* workerVecGet - 
*/
static void workerVecGet(void)
{
	struct stat  statBuf;
	int          s = 0;
	char*        buf;
	int          fileSize;
	int          tot;
	int          nb;
	int          fd;
	bool         seriousError = true;

	LM_T(LmtWorkerVector, ("Retrieving Worker Vector"));

	if ((fd = open(ppFile, O_RDONLY)) == -1)
		LM_X(1, ("error opening samson platform processes file '%s': %s", ppFile, strerror(errno)));
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
		LM_X(1, ("problems with samson platform processes file '%s'", ppFile));
	else
	{
		LM_T(LmtWorkerVector, ("Retrieving worker vec data from file '%s'", ppFile));

		fileSize = statBuf.st_size;
		buf      = (char*) calloc(1, fileSize);

		if (buf == NULL)
			LM_X(1, ("error allocating room for Worker Vector (%s bytes): %s", fileSize, strerror(errno)));
		LM_T(LmtInit, ("Allocated a buffer of %d bytes for the worker vector", fileSize));

		tot = 0;
		while (tot < fileSize)
		{
			nb = read(fd, &buf[tot], fileSize - tot);
			if (nb == -1)
				LM_X(1, ("Error reading from worker vector file '%s': %s", ppFile, strerror(errno)));
			else if (nb == 0)
				LM_X(1, ("Error reading from worker vector file '%s'", ppFile));

			tot += nb;
		}

		workerVec      = (ss::Message::WorkerVectorData*) buf;
		workerVecSize  = sizeof(ss::Message::WorkerVectorData) + workerVec->workers * sizeof(ss::Message::Worker);

		LM_T(LmtWorkerVector, ("file size: %d", fileSize));
		LM_T(LmtWorkerVector, ("%d workers, each of a size of %d => %d + %d * %d == %d",
			  workerVec->workers,
			  sizeof(workerVec->workerV[0]),
			  sizeof(ss::Message::WorkerVectorData),
			  workerVec->workers,
			  sizeof(workerVec->workerV[0]),
			  sizeof(ss::Message::WorkerVectorData) + workerVec->workers * sizeof(workerVec->workerV[0])));

		if (workerVecSize != fileSize)
			LM_X(1, ("Size of file '%s' (%d bytes) does not match the size of a Worker Vector of %d workers: %d bytes", ppFile, fileSize, workerVec->workers, workerVecSize));

		LM_T(LmtInit, ("Read Workers from '%s' - got %d workers", ppFile, workerVec->workers));
		close(fd);

		LM_T(LmtWorkerVector, ("Got %d workers", workerVec->workers));
		for (int ix = 0; ix < workerVec->workers; ix++)
		{
			ss::Message::Worker* workerP = &workerVec->workerV[ix];

			LM_T(LmtWorkerVector, ("  worker %02d: alias: '%s', name: '%s', host: '%s'. port: %d", ix, workerP->alias, workerP->name, workerP->ip, workerP->port));
		}
	}
}



/* ****************************************************************************
*
* main - main routine for the samsonController
*/
int main(int argC, const char* argV[])
{
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

	LM_T(LmtInit, ("ss::Message::WorkerVectorData: %d", sizeof(ss::Message::WorkerVectorData)));
	workerVecGet();

	LM_T(LmtInit, ("%d workers", workerVec->workers));

	au::LockDebugger::shared();         // Lock usage debugging (necessary here where there is only one thread)
	ss::SamsonSetup::load(workingDir);  // Load setup and create all directories
	ss::DiskManager::shared();          // Disk manager
	ss::MemoryManager::init();          // Memory manager
	ss::ModulesManager::init();			// Init the modules manager
	
	
	
	// Instance of network object and initialization
	// ---------------------------------------------
	ss::Network network(ss::Endpoint::Controller, "Controller", CONTROLLER_PORT, endpoints, workerVec->workers);

	network.initAsSamsonController();
	network.workerVecSet(workerVec, workerVecSize, workerVecSave);
	network.runInBackground();
	
	
	// Instance of the Samson Controller
	// ---------------------------------
	
	ss::SamsonController  controller(&network);

	controller.runBackgroundProcesses();
	controller.touch();
	
	while (true)
		sleep(10000);
}
