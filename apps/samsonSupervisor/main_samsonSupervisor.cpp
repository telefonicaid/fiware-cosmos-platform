#include "baTerm.h"             // baTermSetup
#include "logMsg.h"             // LM_*
#include "parseArgs.h"          // parseArgs

#include "NetworkInterface.h"   // DataReceiverInterface
#include "Endpoint.h"           // Endpoint
#include "Network.h"            // Network



/* ****************************************************************************
*
* Option variables
*/
int              endpoints;



/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
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
* SamsonSupervisor - 
*/
class SamsonSupervisor : public ss::DataReceiverInterface
{
public:
	SamsonSupervisor(ss::Network* nwP) { networkP = nwP; }

	virtual int receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP);

private:
	ss::Network*    networkP;
};



/* ****************************************************************************
*
* help - 
*/
static void help(void)
{
	printf("h - print this help message\n");
	printf("q - quit\n");
	printf("s - start samson\n");
}



/* ****************************************************************************
*
* start - 
*/
static void start(void)
{
	printf("start not implemented - sorry ...\n");
}



/* ****************************************************************************
*
* SamsonSupervisor::receive - 
*/
int SamsonSupervisor::receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP)
{
	ss::Endpoint* ep = networkP->endpointLookup(fromId);

	if (ep == NULL)
		LM_RE(0, ("Cannot find endpoint with id %d", fromId));

	if (ep->type == ss::Endpoint::Fd)
	{
		char* msg = (char*) dataP;

		printf("\n");
		switch (*msg)
		{
		case 'h':
			help();
			break;

		case 's':
			start();
			break;

		case 'q':
			LM_X(0, ("'q' pressed - I quit!"));

		default:
			LM_E(("Key '%c' has no function", *msg));
			help();
		}

		printf("\n");
		return 0;
	}

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
	SamsonSupervisor*      supervisorP;
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

	networkP->init(ss::Endpoint::Supervisor, NULL, 0, NULL);
	supervisorP = new SamsonSupervisor(networkP);

	networkP->setDataReceiver(supervisorP);

	baTermSetup();
	networkP->fdSet(0, "stdin", "stdin");
	networkP->run();

	return 0;
}
