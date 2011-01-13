/* ****************************************************************************
*
* FILE                     Process.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 07 2011
*
*/
#include <string.h>             // memset, strcpy, ...

#include "logMsg.h"             // LM_*
#include "Process.h"            // Own interface



/* ****************************************************************************
*
* static global vars
*/
static Process* process[20];



/* ****************************************************************************
*
* processInit - 
*/
void processInit(void)
{
	memset(process, 0, sizeof(process));
}



/* ****************************************************************************
*
* processAdd - 
*/
Process* processAdd(char* name, char* host, char** args, int argCount)
{
	unsigned int  ix = 0;
	int           argIx;

	LM_M(("Adding process '%s' in host '%s' (and with %d args)", name, host, argCount));

	while ((process[ix] != NULL) && (ix < sizeof(process) / sizeof(process[0])))
		++ix;
	if (ix >= sizeof(process) / sizeof(process[0]))
		LM_X(1, ("No room for more Processes - change and recompile!"));

	process[ix] = (Process*) malloc(sizeof(Process));
	if (process[ix] == NULL)
		LM_X(1, ("malloc: %s", strerror(errno)));

	process[ix]->name = strdup(name);
	process[ix]->host = strdup(host);
	
	process[ix]->argCount = argCount;
	argIx = 0;
	while (argIx < argCount)
	{
		LM_M(("Copying arg %d", argIx));
		process[ix]->arg[argIx] = strdup(args[argIx]);
		LM_M(("arg[%d]: '%s'", argIx, process[ix]->arg[argIx]));
		++argIx;
	}

	LM_M(("Added process %d ('%s') in host '%s'", ix, process[ix]->name, process[ix]->host));

	return process[ix];
}



/* ****************************************************************************
*
* processGet - 
*/
Process* processGet(unsigned int ix)
{
	if (ix > sizeof(process) / sizeof(process[0]))
		LM_X(1, ("cannot return process %d - max process id is %d", sizeof(process) / sizeof(process[0])));

	return process[ix];
}



/* ****************************************************************************
*
* processList - 
*/
void processList(void)
{
	unsigned int ix;

	for (ix = 0; ix < sizeof(process) / sizeof(process[0]); ix++)
	{
		if (process[ix] == NULL)
			continue;

		LM_F(("process %02d: %-20s %-20s   %d args", ix, process[ix]->name, process[ix]->host, process[ix]->argCount));
	}
}
