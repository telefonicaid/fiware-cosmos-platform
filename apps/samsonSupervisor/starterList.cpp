/* ****************************************************************************
*
* FILE                     starterList.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 18 2011
*
*/
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // LMT_*

#include "Starter.h"            // Starter
#include "Spawner.h"            // Spawner
#include "Process.h"            // Process
#include "spawnerList.h"        // spawnerListShow
#include "processList.h"        // processListShow
#include "starterList.h"        // Own interface



/* ****************************************************************************
*
* starterV - 
*/
static Starter**     starterV   = NULL;
static unsigned int  starterMax = 0;



/* ****************************************************************************
*
* starterListInit - 
*/
void starterListInit(unsigned int sMax)
{
	starterMax = sMax;
	starterV   = (Starter**) calloc(starterMax, sizeof(Starter*));
}



/* ****************************************************************************
*
* starterMaxGet - 
*/
unsigned int starterMaxGet(void)
{
	return starterMax;
}



/* ****************************************************************************
*
* starterListGet - 
*/
Starter** starterListGet(void)
{
	return starterV;
}



/* ****************************************************************************
*
* starterListShow - 
*/
void starterListShow(const char* what)
{
	unsigned int  ix;
	Starter*      starter;

	LM_F((""));

	LM_F(("----------- Starter List (%s) -----------", what));
	for (ix = 0; ix < starterMax; ix++)
	{
		if (starterV[ix] == NULL)
			continue;

		starter = starterV[ix];

		if (starter->endpoint)
			LM_F(("  %08p  Starter %02d  %30s-Starter %-30s %s Endpoint at %p", starter, ix, starter->typeName(), starter->name, starter->endpoint->typeName(), starter->endpoint));
		else
			LM_F(("  %08p  Starter %02d  %30s-Starter %-30s NULL Endpoint",     starter, ix, starter->typeName(), starter->name));
	}
	LM_F(("--------------------------------"));

	LM_F((""));
	spawnerListShow(what);
	LM_F((""));
	processListShow(what);
	LM_F((""));
	LM_F(("==============================================================================="));
}



/* ****************************************************************************
*
* starterAdd - 
*/
Starter* starterAdd(Starter* starter)
{
	unsigned int ix;

	for (ix = 0; ix < starterMax; ix++)
	{
		if (starterV[ix] == NULL)
		{
			starterV[ix] = starter;
			starterListShow("Starter Added");
			return starterV[ix];
		}
	}

	LM_X(1, ("No more room in starter vector (%d is max index)", starterMax));
	return NULL;
}



/* ****************************************************************************
*
* starterAdd - 
*/
Starter* starterAdd(Process* processP)
{
	Starter* starter;

	starter = new Starter(processP);
	return starterAdd(starter);
}



/* ****************************************************************************
*
* starterAdd - 
*/
Starter* starterAdd(Spawner* spawnerP)
{
	Starter* starter;

	starter = new Starter(spawnerP);
	return starterAdd(starter);
}



/* ****************************************************************************
*
* starterLookup - 
*/
Starter* starterLookup(ss::Endpoint* ep)
{
	unsigned int  ix;

	LM_M(("Looking for starter with endpoint %p", ep));
	starterListShow("Looking for a starter");

	for (ix = 0; ix < starterMax; ix++)
	{
		if (starterV[ix] == NULL)
			continue;

		LM_M(("*** Comparing endpoints %p and %p", ep, starterV[ix]->endpoint));

		if (starterV[ix]->endpoint == ep)
			return starterV[ix];
	}

#if 0
	char*  host;

	for (ix = 0; ix < starterMax; ix++)
	{
		if (starterV[ix] == NULL)
			continue;

		LM_M(("***  Testing '%s' starter '%s'", starterV[ix]->typeName(), starterV[ix]->name));

		if (starterV[ix]->spawner != NULL)
			host = starterV[ix]->spawner->host;
		else if (starterV[ix]->process != NULL)
			host = starterV[ix]->process->host;
		else
			continue;

		LM_M(("*** Comparing '%s' to '%s'", host, ep->ip.c_str()));
		if (strcmp(host, ep->ip.c_str()) == 0)
		{
			LM_M(("Changing '%s' endpoint %p for '%s' endpoint %p", starterV[ix]->endpoint->typeName(), starterV[ix]->endpoint, ep->typeName(), ep));
			starterV[ix]->endpoint = ep;
			LM_M(("found starter at '%s'", host));
			return starterV[ix];
		}
	}
#endif

	LM_M(("Did not find starter"));
	return NULL;
}



/* ****************************************************************************
*
* starterLookup - 
*/
Starter* starterLookup(Process* process)
{
	unsigned int  ix;

	LM_M(("Looking for starter with process %s at %s", process->name, process->host));
	starterListShow("Looking for a starter with a specific process");

	for (ix = 0; ix < starterMax; ix++)
	{
		if (starterV[ix] == NULL)
			continue;

		if (starterV[ix]->process == process)
			return starterV[ix];
	}

	LM_E(("Not found: starter with process %s at %s", process->name, process->host));
	return NULL;
}



/* ****************************************************************************
*
* starterLookup - 
*/
Starter* starterLookup(Spawner* spawner)
{
	unsigned int  ix;

	LM_M(("Looking for starter with spawner at %s", spawner->host));
	starterListShow("Looking for a starter with a specific spawner");

	for (ix = 0; ix < starterMax; ix++)
	{
		if (starterV[ix] == NULL)
			continue;

		if (starterV[ix]->spawner == spawner)
			return starterV[ix];
	}

	LM_E(("Not found: starter with spawner at %s", spawner->host));
	return NULL;
}
