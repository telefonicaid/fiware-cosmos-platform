/* ****************************************************************************
*
* FILE                     spawnerList.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 18 2011
*
*/
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // LMT_*

#include "Spawner.h"            // Spawner
#include "spawnerList.h"        // Own interface



/* ****************************************************************************
*
* static global vars
*/
static Spawner**     spawnerV    = NULL;
static unsigned int  spawnerMax  = 0;
static unsigned int  spawnerIx   = 0;



/* ****************************************************************************
*
* spawnerListInit - 
*/
void spawnerListInit(unsigned int sMax)
{
	spawnerMax = sMax;
	spawnerV   = (Spawner**) calloc(spawnerMax, sizeof(Spawner*));
}



/* ****************************************************************************
*
* spawnerAdd - 
*/
Spawner* spawnerAdd(Spawner* spawnerP)
{
	LM_T(LMT_SPAWNER_LIST, ("Adding spawner for host '%s'", spawnerP->host));

	if (spawnerIx >= spawnerMax)
		LM_X(1, ("No room for more Spawneres (max index is %d) - change and recompile!", spawnerMax));

	if (spawnerLookup(spawnerP->host) != NULL)
		LM_X(1, ("spawner for host '%s' already in spawner list", spawnerP->host));

	spawnerV[spawnerIx] = spawnerP;

	LM_T(LMT_SPAWNER_LIST, ("Spawner for '%s' added", spawnerP->host));

	++spawnerIx;
	return spawnerP;
}



/* ****************************************************************************
*
* spawnerAdd - 
*/
Spawner* spawnerAdd(char* host, unsigned short port, int fd)
{
	Spawner*      spawnerP;

	spawnerP = (Spawner*) malloc(sizeof(Spawner));
	if (spawnerP == NULL)
		LM_X(1, ("malloc: %s", strerror(errno)));

	spawnerP->host  = strdup(host);
	spawnerP->port  = port;
	spawnerP->fd    = fd;

	return spawnerAdd(spawnerP);
}



/* ****************************************************************************
*
* spawnerLookup - 
*/
Spawner* spawnerLookup(unsigned int ix)
{
	if (ix > spawnerMax)
		LM_X(1, ("cannot return spawner %d - max spawner id is %d", spawnerMax));

	return spawnerV[ix];
}



/* ****************************************************************************
*
* spawnerLookup - 
*/
Spawner* spawnerLookup(char* host)
{
	for (unsigned int ix = 0; ix < spawnerMax; ix++)
	{
		if (spawnerV[ix] == NULL)
			continue;

		LM_T(LMT_SPAWNER_LIST, ("[%d] Comparing '%s' ...", ix, host));
		LM_T(LMT_SPAWNER_LIST, ("... to '%s'", spawnerV[ix]->host));

		if (strcmp(spawnerV[ix]->host, host) == 0)
			return spawnerV[ix];

		++ix;
	}

	return NULL;
}



/* ****************************************************************************
*
* spawnerMaxGet - 
*/
unsigned int spawnerMaxGet(void)
{
	return spawnerMax;
}



/* ****************************************************************************
*
* spawnerListGet - 
*/
Spawner** spawnerListGet(void)
{
	return spawnerV;
}



/* ****************************************************************************
*
* spawnerListShow - 
*/
void spawnerListShow(const char* why)
{
	LM_F(("---------- Spawner List: %s ----------", why));
 
	for (unsigned int ix = 0; ix < spawnerMax; ix++)
	{
		if (spawnerV[ix] == NULL)
			continue;

		LM_F(("spawner %02d: %-30s %05d", ix, spawnerV[ix]->host, spawnerV[ix]->port));
	}
}
