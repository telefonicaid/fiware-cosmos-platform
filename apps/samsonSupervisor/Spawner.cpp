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



/* ****************************************************************************
*
* spawnerInit - 
*/
void spawnerInit(void)
{
	memset(spawner, 0, sizeof(spawner));
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

	spawner[ix]->host = strdup(host);
	spawner[ix]->port = port;
	spawner[ix]->fd   = fd;

	LM_M(("Added spawner %d@%s", spawner[ix]->port, spawner[ix]->host));

	return spawner[ix];
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
		if (spawner[ix] != NULL)
		{
			LM_M(("[%d] Comparing '%s' ...", ix, host));
			LM_M(("... to '%s'", spawner[ix]->host));
			if (strcmp(spawner[ix]->host, host) == 0)
				return spawner[ix];
		}

		++ix;
	}

	return NULL;
}



/* ****************************************************************************
*
* spawnerList - 
*/
void spawnerList(void)
{
	unsigned int ix;

	for (ix = 0; ix < sizeof(spawner) / sizeof(spawner[0]); ix++)
	{
		if (spawner[ix] == NULL)
			continue;

		LM_F(("spawner %02d: %-20s %05d", ix, spawner[ix]->host, spawner[ix]->port));
	}
}
