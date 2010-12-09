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
	{ "-port",       &port,        "PORT",        PaShortU,  PaReq,  PaND,   1025,  65000,  "listen port"         },
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

	virtual int receive(int fromId, ss::Message::Header* headerP, void* dataP);

private:
	ss::Network*    networkP;
};



/* ****************************************************************************
*
* SamsonSpawner::receive - 
*/
int SamsonSpawner::receive(int fromId, ss::Message::Header* headerP, void* dataP)
{
	switch (headerP->code)
	{
	case ss::Message::WorkerSpawn:
	case ss::Message::ControllerSpawn:

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

	paParse(paArgs, argC, (char**) argV, 1, true);

	networkP = new ss::Network(endpoints, 0);
	niP = networkP;

	networkP->init(ss::Endpoint::Spawner, NULL, port, NULL);
	spawnerP = new SamsonSpawner(networkP);

	networkP->setDataReceiver(spawnerP);
	networkP->run();

	return 0;
}
