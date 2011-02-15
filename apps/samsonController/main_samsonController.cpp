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

#include "ports.h"              // WORKER_PORT
#include "samsonDirectories.h"  // SAMSON_SETUP_FILE
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
unsigned short   port;
int              endpoints;
int              workers;
char			 workingDir[1024];
char			 workerVecFile[1024];



#define DEF_WD   _i SAMSON_DEFAULT_WORKING_DIRECTORY
#define DEF_WF   _i "/opt/samson/etc/workerVec"
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-working",        workingDir,    "WORKING",         PaString, PaOpt, DEF_WD,  PaNL,  PaNL, "Working directory"   },
	{ "-workerVecFile",  workerVecFile, "WORKERS_FILE",    PaString, PaOpt, DEF_WF,  PaNL,  PaNL, "WorkerVec file"      },
	{ "-port",          &port,          "PORT",            PaShortU, PaOpt,   1234,  1025, 65000, "listen port"         },
	{ "-endpoints",     &endpoints,     "ENDPOINTS",       PaInt,    PaOpt,     80,     3,   100, "number of endpoints" },
	{ "-workers",       &workers,       "WORKERS",         PaInt,    PaOpt,      5,     1,   100, "number of workers"   },

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

	LM_M(("Saving Worker Vector of %d workers", workerVec->workers));
	LM_M(("sizeof(Worker): %d", sizeof(workerVec->workerV[0])));
	LM_M(("file size should be: %d + %d * %d == %d",
		  sizeof(ss::Message::WorkerVectorData),
		  workerVec->workers,
		  sizeof(workerVec->workerV[0]),
		  sizeof(ss::Message::WorkerVectorData) + workerVec->workers * sizeof(workerVec->workerV[0])));

	if ((fd = open(workerVecFile, O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU)) == -1)
		LM_RVE(("open-for-writing(%s): %s", workerVecFile, strerror(errno)));

	buf  = (char*) workerVec;
	tot  = 0;

	while (tot < workerVecSize)
	{
		nb = write(fd, &buf[tot], workerVecSize - tot);
		if (nb == -1)
		{
			close(fd);
			unlink(workerVecFile);
			LM_RVE(("write(%d bytes to '%s'): %s", workerVecSize - tot, workerVecFile, strerror(errno)));
		}
		else if (nb == 0)
		{
			close(fd);
			unlink(workerVecFile);
			LM_RVE(("write(ZERO bytes to '%s'): %s", workerVecFile, strerror(errno)));
		}

		tot += nb;
	}

	close(fd);
	
	if (chmod(workerVecFile, S_IRWXU) != 0)
		LM_E(("chmod(%s): %s", workerVecFile, strerror(errno)));
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

	LM_M(("Retrieving Worker Vector"));

	if ((fd = open(workerVecFile, O_RDONLY)) == -1)
		LM_E(("open-for-reading(%s): %s - this is OK iff file-doesn't-exist", workerVecFile, strerror(errno)));
	else if ((s = stat(workerVecFile, &statBuf)) == -1)
		LM_E(("stat(%s): %s", workerVecFile, strerror(errno)));

	if ((s == -1) || (fd == -1) || (statBuf.st_size == 0))
	{
		if (fd != -1)
			close(fd);

		if ((fd = open(workerVecFile, O_WRONLY | O_CREAT)) == -1)
			LM_X(1, ("open-for-writing(%s): %s", workerVecFile, strerror(errno)));

		LM_M(("Inventing Worker Vector with %d workers (number came from command line options)", workers));

		LM_W(("Problems with config file '%s' - using %d empty workers (according to command line options)", workerVecFile, workers));
		LM_W(("This just might be a serious problem. Perhaps I should enter a 'semi sleep' mode, until samsonSupervisor sends info on worker vector (like 'first start')"));
		LM_W(("At least, the number of workers came from command line came from samsonSupervisor (supposing that the cpontroller wasn't started by hand ...)"));

		workerVecSize = sizeof(ss::Message::WorkerVectorData) + workers * sizeof(ss::Message::Worker);
		workerVec     = (ss::Message::WorkerVectorData*) malloc(workerVecSize);

		if (workerVec == NULL)
			LM_X(1, ("error allocating room for Worker Vector (%s bytes): %s", workerVecSize, strerror(errno)));

		memset(workerVec, 0, workerVecSize);
		workerVec->workers = workers;
		LM_M(("Using global option variable 'workers' to decide how many workers I use: %d workers", workers));

		for (int ix = 0; ix < workerVec->workers; ix++)
		{
			ss::Message::Worker* worker = &workerVec->workerV[ix];

			snprintf(worker->name, sizeof(worker->name),   "Worker");
			snprintf(worker->alias, sizeof(worker->alias), "Worker%02d", ix);

			worker->port  = WORKER_PORT;
			worker->state = ss::Endpoint::FutureWorker;
		}

		workerVecSave();
		close(fd);
	}
	else
	{
		LM_M(("Retrieving worker vec data from file '%s'", workerVecFile));

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
				LM_X(1, ("Error reading from worker vector file '%s': %s", workerVecFile, strerror(errno)));
			else if (nb == 0)
				LM_X(1, ("Error reading from worker vector file '%s'", workerVecFile));

			tot += nb;
		}

		workerVec      = (ss::Message::WorkerVectorData*) buf;
		workerVecSize  = sizeof(ss::Message::WorkerVectorData) + workerVec->workers * sizeof(ss::Message::Worker);

		LM_M(("Changing global variable workers from %d to %d", workers, workerVec->workers));
		workers        = workerVec->workers;

		
		LM_M(("file size: %d", fileSize));
		LM_M(("%d workers, each of a size of %d => %d + %d * %d == %d",
			  workerVec->workers,
			  sizeof(workerVec->workerV[0]),
			  sizeof(ss::Message::WorkerVectorData),
			  workerVec->workers,
			  sizeof(workerVec->workerV[0]),
			  sizeof(ss::Message::WorkerVectorData) + workerVec->workers * sizeof(workerVec->workerV[0])));

		if (workerVecSize != fileSize)
			LM_X(1, ("Size of file '%s' (%d bytes) does not match the size of a Worker Vector of %d workers: %d bytes", workerVecFile, fileSize, workerVec->workers, workerVecSize));

		LM_T(LmtInit, ("Read Workers from '%s' - got %d workers", workerVecFile, workerVec->workers));
		close(fd);

		LM_M(("Got %d workers", workerVec->workers));
		for (int ix = 0; ix < workerVec->workers; ix++)
		{
			ss::Message::Worker* workerP = &workerVec->workerV[ix];

			LM_M(("  worker %02d: alias: '%s', name: '%s', host: '%s'. port: %d", ix, workerP->alias, workerP->name, workerP->ip, workerP->port));
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

	au::LockDebugger::shared();         // Debuggin of Lock usage ( necessary here where it is only one thread )
	ss::SamsonSetup::load(workingDir);  // Load setup and create all directories
	ss::DiskManager::shared();          // Disk manager
	ss::MemoryManager::init();          // Memory manager

	
	
	
	// Instance of network object and initialization
	// --------------------------------------------------------------------
	ss::Network network(ss::Endpoint::Controller, "controller", port, endpoints, workers);

	network.initAsSamsonController();
	network.workerVecSet(workerVec, workerVecSize, workerVecSave);
	network.runInBackground();
	
	
	// Instance of the samson controller	
	// --------------------------------------------------------------------
	
	ss::SamsonController  controller(&network);

	controller.runBackgroundProcesses();
	controller.touch();
	
	while (true)
		sleep(10000);
}
