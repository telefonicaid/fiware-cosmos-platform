/* ****************************************************************************
*
* FILE                     main_samsonKiller.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 25 2011
*
*/
#include "parseArgs.h"          // parseArgs
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace levels

#include "Endpoint.h"           // Endpoint
#include "iomConnect.h"         // iomConnect
#include "iomMsgSend.h"         // iomMsgSend




/* ****************************************************************************
*
* Option variables
*/
unsigned short   port;



/* ****************************************************************************
*
* parse arguments
*/
PaArgument paArgs[] =
{
	{ "-port",       &port,        "PORT",        PaShortU,  PaOpt,  1233,   1025,  65000,  "listen port"         },

	PA_END_OF_ARGS
};



/* ****************************************************************************
*
* main - 
*/
int main(int argC, const char *argV[])
{
	ss::Endpoint  ep;
	ss::Endpoint  me;

	close(0);
	close(1);
	close(2);

	paParse(paArgs, argC, (char**) argV, 1, false);

	LM_T(LmtInit, ("Started with arguments:"));
	for (int ix = 0; ix < argC; ix++)
		LM_T(LmtInit, ("  %02d: '%s'", ix, argV[ix]));

	ep.name = "toBeKilled";
	ep.wFd = iomConnect("localhost", port);
	
	if (ep.wFd == -1)
	   LM_X(0, ("Error connecting to 'localhost', port %d - probably means the process isn't running", port));

	me.name = "samsonKiller";

	if (iomMsgSend(&ep, &me, ss::Message::Die, ss::Message::Msg) != 0)
		LM_X(1, ("error sending 'Die' message to endpoint (port %d)", port));

	return 0;
}
