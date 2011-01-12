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
static Spawner* spawner[20];
static Process* process[20];



/* ****************************************************************************
*
* spawnerInit - 
*/
void spawnerInit(void)
{
	memset(spawner, 0, sizeof(spawner));
	memset(process, 0, sizeof(process));
}



/* ****************************************************************************
*
* spawnerAdd - 
*/
Spawner* spawnerAdd(char* host, unsigned short port, int fd)
{
	unsigned int ix = 0;

	while ((spawner[ix] != NULL) && (ix < sizeof(spawner) / sizeof(spawner[0])))
		++ix;
	if (ix >= sizeof(spawner) / sizeof(spawner[0]))
		LM_X(1, ("No room for more Spawners - change and recompile!"));

	spawner[ix] = (Spawner*) malloc(sizeof(Spawner));
	if (spawner[ix] == NULL)
		LM_X(1, ("malloc: %s", strerror(errno)));

	strcpy(spawner[ix]->host, host);
	spawner[ix]->port = port;
	spawner[ix]->fd   = fd;

	return spawner[ix];
}



/* ****************************************************************************
*
* processAdd - 
*/
Process* processAdd(char* name, char* host, unsigned short port, char** args, int argCount)
{
	unsigned int  ix = 0;
	int           argIx;

	while ((process[ix] != NULL) && (ix < sizeof(process) / sizeof(process[0])))
		++ix;
	if (ix >= sizeof(process) / sizeof(process[0]))
		LM_X(1, ("No room for more Processes - change and recompile!"));

	process[ix] = (Process*) malloc(sizeof(Process));
	if (process[ix] == NULL)
		LM_X(1, ("malloc: %s", strerror(errno)));

	strcpy(process[ix]->name, name);
	strcpy(process[ix]->host, host);
	process[ix]->port = port;
	
	process[ix]->argCount = argCount;
	while (argIx < argCount)
	{
		process[ix]->arg[argIx] = strdup(args[argIx]);
		++argIx;
	}

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
* spawnerGet - 
*/
Spawner* spawnerGet(char* host)
{
	unsigned int ix = 0;

	while (ix < sizeof(spawner) / sizeof(spawner[0]))
	{
		if (strcmp(spawner[ix]->host, host) == 0)
			return spawner[ix];
	}

	return NULL;
}
