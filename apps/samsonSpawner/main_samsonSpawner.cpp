#include <unistd.h>             // fork & exec

#include "logMsg.h"             // LM_*
#include "parseArgs.h"          // parseArgs

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
int                logServerFd       = -1;    // socket to send Log Lines to LogServer
ss::Endpoint*      logServerEndpoint = NULL;
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
		LM_M(("arg %d: '%s'", arg, args[arg]));
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
		LM_M(("Got a '%s' spawn message (with %d parameters)", ss::Message::messageCode(headerP->code), argCount));

		spawnParse(spawnData, args, argCount);
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
* logHookFunction - 
*/
void logHookFunction(char* text, char type, const char* file, int lineNo, const char* fName, int tLev, const char* stre)
{
	int                       s;
	ss::Message::LogLineData  logLine;
	ss::Endpoint*             ep;

	if (networkP == NULL)
		return;

	if ((ep = networkP->logServerLookup()) == NULL)
		return;

	memset(&logLine, 0, sizeof(logLine));

	logLine.type   = type;
	logLine.lineNo = lineNo;
	logLine.tLev   = tLev;

	if (text  != NULL)  strncpy(logLine.text,  text,  sizeof(logLine.text)  - 1);
	if (file  != NULL)  strncpy(logLine.file,  file,  sizeof(logLine.file)  - 1);
	if (fName != NULL)  strncpy(logLine.fName, fName, sizeof(logLine.fName) - 1);
	if (stre  != NULL)  strncpy(logLine.stre,  stre,  sizeof(logLine.stre));

	if (ep)
		s = iomMsgSend(ep->wFd, ep->name.c_str(), networkP->me->name.c_str(), ss::Message::LogLine, ss::Message::Msg, &logLine, sizeof(logLine), NULL);
}



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	SamsonSpawner*         spawnerP;
	ss::NetworkInterface*  niP;

	paConfig("prefix",                        (void*) "SSS_");
	paConfig("usage and exit on any warning", (void*) true);
	paConfig("log to screen",                 (void*) "only errors");
	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("screen line format",            (void*) "TYPE: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	if (strcmp(logServer, (char*) NOLS) != 0)
	{
		LM_M(("Connecting to log server '%s' at port %d", logServer, LOG_SERVER_PORT));
		logServerFd = iomConnect(logServer, LOG_SERVER_PORT);
	}

	networkP = new ss::Network(endpoints, 0);
	niP = networkP;

	networkP->init(ss::Endpoint::Spawner, NULL, port, NULL);

	lmOutHookSet(logHookFunction);

	spawnerP = new SamsonSpawner(networkP);

	networkP->setDataReceiver(spawnerP);

	if (logServerFd != -1)
	{
		logServerEndpoint = networkP->endpointAdd(logServerFd, logServerFd, "Samson Log Server", "logServer", 0, ss::Endpoint::LogServer, "localhost", LOG_SERVER_PORT);
		if (logServerEndpoint)
		{
			LM_M(("Sending Hello to Log Server"));
			networkP->helloSend(logServerEndpoint, ss::Message::Msg);
		}
		else
			LM_E(("Error adding endpoint"));
	}

	networkP->run();

	return 0;
}
