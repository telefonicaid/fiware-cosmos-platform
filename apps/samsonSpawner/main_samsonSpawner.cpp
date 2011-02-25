/* ****************************************************************************
*
* FILE                     main_samsonSpawner.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 14 2010
*
*/
#include <unistd.h>             // fork & exec
#include <sys/types.h>          // pid_t
#include <sys/wait.h>           // waitpid

#include "parseArgs.h"          // parseArgs
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace levels

#include "NetworkInterface.h"   // DataReceiverInterface
#include "Network.h"            // Network
#include "Process.h"            // Process
#include "iomMsgSend.h"         // iomMsgSend
#include "iomConnect.h"         // iomConnect
#include "ports.h"              // LOG_SERVER_PORT
#include "daemonize.h"          // daemonize



/* ****************************************************************************
*
* Option variables
*/
unsigned short   port;
int              endpoints;



#define NOLS (long int) "no log server"
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-port",       &port,        "PORT",        PaShortU,  PaOpt,  1233,   1025,  65000,  "listen port"         },
	{ "-endpoints",  &endpoints,   "ENDPOINTS",   PaInt,     PaOpt,    80,     10,    100,  "number of endpoints" },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* Global variables
*/
int                logFd             = -1;
ss::Network*       networkP          = NULL;



/* ****************************************************************************
*
* SamsonSpawner - 
*/
class SamsonSpawner : public ss::DataReceiverInterface, public ss::TimeoutReceiverInterface
{
public:
	SamsonSpawner(ss::Network* nwP) { networkP = nwP; }
	void processSpawn(ss::Process* processP);

	virtual int receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP);
	virtual int timeoutFunction(void);

private:
	ss::Network*    networkP;
};



/* ****************************************************************************
*
* timeoutFunction - 
*/
int SamsonSpawner::timeoutFunction(void)
{
	pid_t  pid;
	int    status;

	pid = waitpid(-1, &status, WNOHANG);
	if (pid == 0)
		LM_T(LmtWait, ("Children running, no one has exited since last sweep"));
	else if (pid == -1)
	{
		if (errno != ECHILD)
			LM_E(("waitpid error: %s", strerror(errno)));
	}
	else
		LM_W(("Process %d died with status 0x%x", pid, status));

	return 0;
}



/* ****************************************************************************
*
* processSpawn - 
*/
void SamsonSpawner::processSpawn(ss::Process* processP)
{
	pid_t  pid;
	char*  argV[50];
	int    argC = 0;

	if (processP->type == ss::PtWorker)
	{
		argV[argC++] = (char*) "samsonWorker";
		argV[argC++] = (char*) "-alias";
		argV[argC++] = processP->alias;
		argV[argC++] = (char*) "-controller";
		argV[argC++] = (char*) processP->controllerHost;
	}
	else if (processP->type == ss::PtController)
	{
		char workersV[16];

		snprintf(workersV, sizeof(workersV), "%d", processP->workers);

		argV[argC++] = (char*) "samsonController";
		argV[argC++] = (char*) "-workers";
		argV[argC++] = workersV;
	}
	else
		LM_X(1, ("Will only start workers and controllers - bad process type %d", processP->type));

	if (processP->verbose == true)   argV[argC++] = (char*) "-v";
	if (processP->debug   == true)   argV[argC++] = (char*) "-d";
	if (processP->reads   == true)   argV[argC++] = (char*) "-r";
	if (processP->writes  == true)   argV[argC++] = (char*) "-w";
	// if (processP->toDo == true)   argV[argC++] = (char*) "-toDo";

	char traceLevels[512];
	lmTraceGet(traceLevels, sizeof(traceLevels), processP->traceLevels);
	if (traceLevels[0] != 0)
	{
		argV[argC++] = (char*) "-t";
		argV[argC++] = traceLevels;
	}

	argV[argC] = NULL;

	pid = fork();
	if (pid == 0)
	{
		int ix;
		int s;

		LM_V(("Spawning a '%s' with %d parameters", argV[0], argC));

		s = execvp(argV[0], argV);
		if (s == -1)
			LM_E(("Back from EXEC: %s", strerror(errno)));
		else
			LM_E(("Back from EXEC"));

		LM_E(("Tried to start '%s' with the following parameters:", argV[0]));
		for (ix = 0; ix < argC + 1; ix++)
			LM_E(("%02d: %s", ix, argV[ix]));

		LM_X(1, ("Back from EXEC !!!"));
	}
}



/* ****************************************************************************
*
* SamsonSpawner::receive - 
*/
int SamsonSpawner::receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP)
{
	switch (headerP->code)
	{
	case ss::Message::ProcessSpawn:
		processSpawn((ss::Process*) dataP);
		break;

	default:
		LM_X(1, ("Don't know how to treat '%s' message", ss::Message::messageCode(headerP->code)));
	}

	return 0;
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	SamsonSpawner* spawnerP;
	
	daemonize();

	paConfig("prefix",                        (void*) "SSS_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE:EXEC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_T(LmtInit, ("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

	networkP = new ss::Network(ss::Endpoint::Spawner, "Spawner", port, endpoints, 0);
	networkP->init(NULL);

	spawnerP = new SamsonSpawner(networkP);
	networkP->setDataReceiver(spawnerP);
	networkP->setTimeoutReceiver(spawnerP, 1, 0); // Tiemout every second
	networkP->run();

	return 0;
}
