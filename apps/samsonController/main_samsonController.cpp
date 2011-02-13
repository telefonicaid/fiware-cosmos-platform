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
char			 workersFile[1024];



#define DEF_WD   _i SAMSON_DEFAULT_WORKING_DIRECTORY
#define DEF_WF   _i "/opt/samson/etc/workers"
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-working",       workingDir,  "WORKING",         PaString, PaOpt, DEF_WD,  PaNL,  PaNL, "Working directory"   },
	{ "-workersFile",   workersFile, "WORKERS_FILE",    PaString, PaOpt, DEF_WF,  PaNL,  PaNL, "Working directory"   },
	{ "-port",         &port,        "PORT",            PaShortU, PaOpt,   1234,  1025, 65000, "listen port"         },
	{ "-endpoints",    &endpoints,   "ENDPOINTS",       PaInt,    PaOpt,     80,     3,   100, "number of endpoints" },
	{ "-workers",      &workers,     "WORKERS",         PaInt,    PaOpt,      5,     1,   100, "number of workers"   },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* logFd - file descriptor for log file used in all libraries
*/
int                             logFd      = -1;
ss::Message::WorkerVectorData*  workerVec  = NULL;



/* ****************************************************************************
*
* workersGet - 
*/
static ss::Message::WorkerVectorData* workersGet(char* path, int* sizeP)
{
	struct stat  statBuf;
	int          s = 0;
	char*        buf;
	int          fileSize;
	int          tot;
	int          nb;
	int          fd;
	int          size;

	if ((fd = open(path, O_RDONLY)) == -1)
		LM_E(("open(%s): %s", strerror(errno)));
	else if ((s = stat(path, &statBuf)) == -1)
		LM_E(("stat(%s): %s", strerror(errno)));

	if ((s == -1) || (fd == -1))
	{
		ss::Message::Worker* worker;

		LM_W(("Problems with config file '%s' - using %d empty workers (according to command line options)", path, workers));
		LM_W(("This actually may be a serious problem. Perhaps I should enter a 'semi sleep' mode, until samsonSupervisor sends info on worker vector (like 'first start')"));
		LM_W(("At least, the number of worler vectors from command line came from samsonSupervisor - supposing that the cpontroller wasn't started by hand ..."));

		size = sizeof(ss::Message::WorkerVectorData) + workers * sizeof(ss::Message::Worker);
		workerVec = (ss::Message::WorkerVectorData*) malloc(size);
		if (workerVec == NULL)
			LM_X(1, ("error allocating room for Worker Vector (%s bytes): %s", size, strerror(errno)));

		memset(workerVec, 0, size);
		workerVec->workers = workers;
		*sizeP = size;

		for (int ix = 0; ix < workers; ix++)
		{
			worker = &workerVec->workerV[ix];

			snprintf(worker->name, sizeof(worker->name),   "samsonWorker");
			snprintf(worker->alias, sizeof(worker->alias), "Worker%02d", ix);
			worker->port  = WORKER_PORT;
			worker->state = ss::Endpoint::FutureWorker;
		}

		return workerVec;
	}


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
			LM_X(1, ("Error reading from worker vector file '%s': %s", path, strerror(errno)));
		else if (nb == 0)
			LM_X(1, ("Error reading from worker vector file '%s'", path));

		tot += nb;
	}

	workerVec = (ss::Message::WorkerVectorData*) buf;
	size      = sizeof(ss::Message::WorkerVectorData) + workerVec->workers * sizeof(ss::Message::Worker);

	if (size != fileSize)
		LM_X(1, ("Size of file '%s' (%d bytes) does not match the size of a Worker Vector of %d workers: %d bytes", path, fileSize, workerVec->workers, size));

	LM_T(LmtInit, ("Read Workers from '%s' - got %d workers", path, workerVec->workers));
	*sizeP = size;
	return workerVec;

}



/* ****************************************************************************
*
* main - main routine for the samsonController
*/
int main(int argC, const char* argV[])
{
	ss::Message::WorkerVectorData*  workerVec;
	int                             workerVecSize;

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
	workerVec = workersGet(workersFile, &workerVecSize);

	LM_T(LmtInit, ("%d workers", workerVec->workers));

	au::LockDebugger::shared();         // Debuggin of Lock usage ( necessary here where it is only one thread )
	ss::SamsonSetup::load(workingDir);  // Load setup and create all directories
	ss::DiskManager::shared();          // Disk manager
	ss::MemoryManager::init();          // Memory manager

	
	
	
	// Instance of network object and initialization
	// --------------------------------------------------------------------
	ss::Network network(ss::Endpoint::Controller, "controller", port, endpoints, workers);

	network.initAsSamsonController();
	network.workerVecSet(workerVec, workerVecSize);
	network.runInBackground();
	
	
	// Instance of the samson controller	
	// --------------------------------------------------------------------
	
	ss::SamsonController  controller(&network);

	controller.runBackgroundProcesses();
	controller.touch();
	
	while (true)
		sleep(10000);
}
