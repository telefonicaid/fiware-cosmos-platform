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
#include "traceLevels.h"        // Lmt*

#include "Starter.h"            // Starter
#include "Process.h"            // Process
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

	LM_T(LmtStarterListShow, (""));

	LM_T(LmtStarterListShow, ("----------- Starter List (%s) -----------", what));
	for (ix = 0; ix < starterMax; ix++)
	{
		if (starterV[ix] == NULL)
			continue;

		starter = starterV[ix];

		if (starter->process->endpoint)
		   LM_T(LmtStarterListShow, ("  %08p  Starter %02d  %30s-Starter %-30s (process at %p) %s Endpoint at %p", starter, ix, processTypeName(starter->process), starter->process->name, starter->process, starter->process->endpoint->typeName(), starter->process->endpoint));
		else
		   LM_T(LmtStarterListShow, ("  %08p  Starter %02d  %30s-Starter %-30s (process at %p) NULL Endpoint",     starter, ix, processTypeName(starter->process), starter->process->name, starter->process));
	}
	LM_T(LmtStarterListShow, ("--------------------------------"));

	LM_T(LmtStarterListShow, (""));
	processListShow(what);
	LM_T(LmtStarterListShow, (""));
	LM_T(LmtStarterListShow, ("==============================================================================="));
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

	if (processP->starterP != NULL)
	{
		LM_T(LmtStarter, ("process '%s@%s' already has a starter", processP->name, processP->host));
		return processP->starterP;
	}
	
	LM_T(LmtStarter, ("Allocating a starter for '%s@%s'", processP->name, processP->host));

	starter = new Starter(processP);
	processP->starterP = starter;

	return starterAdd(starter);
}



/* ****************************************************************************
*
* starterLookup - 
*/
Starter* starterLookup(ss::Endpoint* ep)
{
	unsigned int  ix;

	LM_T(LmtStarterLookup, ("Looking for starter with endpoint %p", ep));
	starterListShow("Looking for a starter");

	for (ix = 0; ix < starterMax; ix++)
	{
		if (starterV[ix] == NULL)
			continue;

		LM_T(LmtStarterLookup, ("*** Comparing endpoints %p and %p", ep, starterV[ix]->process->endpoint));

		if (starterV[ix]->process->endpoint == ep)
			return starterV[ix];
	}

	LM_T(LmtStarterLookup, ("Did not find starter with endpoint %p", ep));
	return NULL;
}



/* ****************************************************************************
*
* starterLookup - 
*/
Starter* starterLookup(Process* process)
{
	unsigned int  ix;

	LM_T(LmtStarterLookup, ("Looking for starter with process %s at %s", process->name, process->host));
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
