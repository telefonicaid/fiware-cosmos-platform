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

#include "parseArgs.h"          // parseArgs
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace levels

#include "NetworkInterface.h"   // DataReceiverInterface
#include "Network.h"            // Network
#include "Process.h"            // Process
#include "iomMsgSend.h"         // iomMsgSend
#include "iomConnect.h"         // iomConnect
#include "ports.h"              // LOG_SERVER_PORT




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
	{ "-endpoints",  &endpoints,   "ENDPOINTS",   PaInt,     PaOpt,    20,      3,    100,  "number of endpoints" },

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
class SamsonSpawner : public ss::DataReceiverInterface
{
public:
	SamsonSpawner(ss::Network* nwP) { networkP = nwP; }
	void processSpawn(ss::Process* processP);

	virtual int receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP);

private:
	ss::Network*    networkP;
};



/* ****************************************************************************
*
* spawnParse - 
*/
void spawnParse(ss::Message::SpawnData* spawnData, char** args, int argCount)
{
	int    arg = 0;
	char*  end;

	end = spawnData->args;
	while (1)
	{
	   if ((end[0] == 0) && (end[1] == 0))
			break;

		args[arg] = end;
		LM_T(LmtInit, ("arg %d: '%s'", arg, args[arg]));
		end = end + strlen(args[arg]) + 1;
		++arg;

		if (arg == argCount)
		{
			args[arg] = NULL;
			break;
		}
	}
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

	if (processP->type == ss::PtWorkerStarter)
	{
		argV[argC++] = (char*) "samsonWorker";
		argV[argC++] = (char*) "-alias";
		argV[argC++] = processP->alias;
		argV[argC++] = (char*) "-controller";
		argV[argC++] = (char*) processP->controllerHost;
	}
	else if (processP->type == ss::PtControllerStarter)
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
	char*  args[20];
	int    argCount;
	ss::Message::SpawnData* spawnData;

	switch (headerP->code)
	{
	case ss::Message::ProcessSpawn:
		processSpawn((ss::Process*) dataP);
		break;

	case ss::Message::WorkerSpawn:
	case ss::Message::ControllerSpawn:
		pid_t  pid;
		char*  evec[21];

		spawnData = (ss::Message::SpawnData*) dataP;
		argCount = spawnData->argCount;
		LM_T(LmtSpawn, ("Got a '%s' spawn message (with %d parameters)", ss::Message::messageCode(headerP->code), argCount));

		spawnParse(spawnData, args, argCount);
		LM_T(LmtSpawn, ("Spawning %s with %d parameters:", (headerP->code == ss::Message::WorkerSpawn)? "samsonWorker" : "samsonController", argCount));
		for (int ix = 0; ix < argCount; ix++)
			LM_T(LmtSpawn, ("o '%s'", args[ix]));

		pid = fork();
		if (pid == 0)
		{
			int ix;
			int s;

			evec[0] = (headerP->code == ss::Message::WorkerSpawn)? (char*) "samsonWorker" : (char*) "samsonController";
			
			LM_V(("exec arg 0 (prog name): '%s'", evec[0]));
			for (ix = 0; ix < argCount; ix++)
			{
				evec[ix + 1] = args[ix];
				LM_V(("exec arg %02d: '%s'", ix, evec[ix + 1]));
			}

			evec[ix + 1] = 0;

			LM_T(LmtSpawn, ("execvp(%s, %s, %s, %s, ...", evec[0], evec[0], evec[1], evec[2]));
			s = execvp(evec[0], evec);
			if (s == -1)
			   LM_E(("Back from EXEC: %s", strerror(errno)));
			else
			   LM_E(("Back from EXEC"));

			LM_E(("Tried to start '%s' with the following parameters:", evec[0]));
			for (ix = 0; ix < argCount + 1; ix++)
				LM_E(("%02d: %s", ix, evec[ix]));

			LM_X(1, ("Back from EXEC !!!"));
		}
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
	SamsonSpawner*         spawnerP;
	
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

	networkP = new ss::Network(ss::Endpoint::Spawner, NULL, port, endpoints, 0);
	networkP->init(NULL);

	spawnerP = new SamsonSpawner(networkP);
	networkP->setDataReceiver(spawnerP);
	networkP->run();

	return 0;
}
