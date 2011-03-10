#include <unistd.h>             // gethostname

#include "parseArgs.h"          // parseArgs
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace levels

#include "Host.h"               // Host
#include "HostMgr.h"            // Network



/* ****************************************************************************
*
* External declarations
*/
extern "C" void exit(int);



/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* main - 
*/
int main(int argC, char* argV[])
{
	ss::HostMgr*  hostMgr = new ss::HostMgr(10);
	Host*         hostP;
	Host*         localhostP;
	char          hostname[128];

	paConfig("log file line format",          (void*) "TYPE:DATE:EXEC-AUX/FILE[LINE] FUNC: TEXT");
	paConfig("log to file",                   (void*) true);

	paParse(paArgs, argC, (char**) argV, 1, false);

	hostP = hostMgr->lookup("localhost");
	if (hostP == NULL)
		LM_X(1, ("Cannot find 'localhost'"));

	gethostname(hostname, sizeof(hostname));
	localhostP = hostMgr->insert(hostname, NULL);
	if (localhostP != hostP)
		LM_X(1, ("Inserted localhost '%s' again ...", hostname));

	hostP = hostMgr->lookup("localhost");
	if (hostP == NULL)
        LM_X(1, ("Cannot find 'localhost'"));

	LM_M(("localhost:  name:'%s', ip:'%s'", hostP->name, hostP->ip));

	LM_M(("Test OK"));
	return 0;
}
