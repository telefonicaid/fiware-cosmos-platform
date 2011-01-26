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
#include "iomMsgSend.h"         // iomMsgSend
#include "iomConnect.h"         // iomConnect
#include "ports.h"              // LOG_SERVER_PORT




/* ****************************************************************************
*
* Option variables
*/
unsigned short   port;
int              endpoints;
char             logServer[80];



#define NOLS (long int) "no log server"
/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-port",       &port,        "PORT",        PaShortU,  PaOpt,  1233,   1025,  65000,  "listen port"         },
	{ "-endpoints",  &endpoints,   "ENDPOINTS",   PaInt,     PaOpt,    20,      3,    100,  "number of endpoints" },
	{ "-logServer",  logServer,    "LOG_SERVER",  PaString,  PaOpt,  NOLS,   PaNL,   PaNL,  "log server host"     },

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
* SamsonSpawner::receive - 
*/
int SamsonSpawner::receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP)
{
	char*  args[20];
	int    argCount;
	ss::Message::SpawnData* spawnData;

	switch (headerP->code)
	{
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

			evec[0] = (headerP->code == ss::Message::WorkerSpawn)? (char*) "samsonWorker" : (char*) "samsonController";
			
			for (ix = 0; ix < argCount; ix++)
				evec[ix + 1] = args[ix];
			evec[ix + 1] = 0;

			LM_T(LmtSpawn, ("execvp(%s, %s, %s, %s, ...", evec[0], evec[0], evec[1], evec[2]));
			execvp(evec[0], evec);
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

	networkP = new ss::Network(endpoints, 0);
	networkP->init(ss::Endpoint::Spawner, NULL, port, NULL);
	networkP->logServerSet(logServer);
	spawnerP = new SamsonSpawner(networkP);
	networkP->setDataReceiver(spawnerP);
	networkP->run();

	return 0;
}
