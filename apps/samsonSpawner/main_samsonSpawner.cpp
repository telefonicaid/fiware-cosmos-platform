#include <unistd.h>             // fork & exec

#include "logMsg.h"             // LM_*
#include "parseArgs.h"          // parseArgs
#include "NetworkInterface.h"   // DataReceiverInterface
#include "Network.h"            // Network




/* ****************************************************************************
*
* Option variables
*/
unsigned short   port;
int              endpoints;



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
* logFd - file descriptor for log file used in all libraries
*/
int logFd = -1;



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
void spawnParse(ss::Message::SpawnData* spawnData, char** args, int* argCountP)
{
	int    arg = 0;
	char*  end;

	end = spawnData->args;
	while (1)
	{
	   if ((end[0] == 0) && (end[1] == 0))
			break;

		args[arg] = end;
		end = end + strlen(args[arg]) + 1;
		++arg;
	}

	*argCountP = arg;
}



/* ****************************************************************************
*
* SamsonSpawner::receive - 
*/
int SamsonSpawner::receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP)
{
	char*  args[20];
	int    argCount;

	switch (headerP->code)
	{
	case ss::Message::WorkerSpawn:
	case ss::Message::ControllerSpawn:
		pid_t  pid;
		char*  evec[21];


		LM_M(("Got a '%s' message.", ss::Message::messageCode(headerP->code)));
		spawnParse((ss::Message::SpawnData*) dataP, args, &argCount);

		LM_M(("Spawning %s with %d parameters:", (headerP->code == ss::Message::WorkerSpawn)? "samsonWorker" : "samsonController", argCount));
		for (int ix = 0; ix < argCount; ix++)
			LM_M(("o '%s'", args[ix]));

		pid = fork();
		if (pid == 0)
		{
			int ix;

			evec[0] = (headerP->code == ss::Message::WorkerSpawn)? (char*) "samsonWorker" : (char*) "samsonController";
			
			for (ix = 0; ix < argCount; ix++)
				evec[ix + 1] = args[ix];
			evec[ix + 1] = 0;

			LM_M(("execvp(%s, %s, %s, %s, ...", evec[0], evec[0], evec[1], evec[2]));
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
	ss::Network*           networkP;
	SamsonSpawner*         spawnerP;
	ss::NetworkInterface*  niP;

	paConfig("prefix",                        (void*) "SSS_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	networkP = new ss::Network(endpoints, 0);
	niP = networkP;

	networkP->init(ss::Endpoint::Spawner, NULL, port, NULL);
	spawnerP = new SamsonSpawner(networkP);

	networkP->setDataReceiver(spawnerP);
	networkP->run();

	return 0;
}
