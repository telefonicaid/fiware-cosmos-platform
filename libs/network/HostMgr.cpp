/* ****************************************************************************
*
* FILE                     HostMgr.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 10 2011
*
*/
#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "Host.h"               // Host
#include "HostMgr.h"            // Own interface



/* ****************************************************************************
*
* HostMgr
*/
HostMgr::HostMgr(unsigned int size)
{
	this->size = size;

	hostV = (Host**) calloc(size, sizeof(Host*));
	if (hostV == NULL)
		LM_X(1, ("error allocating room for %d delilah hosts", size));
}



/* ****************************************************************************
*
* HostMgr::hosts - 
*/
int HostMgr::hosts(void)
{
	unsigned int  ix;
	int           hostNo = 0;

	for (ix = 0; ix < size; ix++)
	{
		if (hostV[ix] != NULL)
			++hostNo;
	}

	return hostNo;
}



/* ****************************************************************************
*
* HostMgr::insert - 
*/
void HostMgr::insert(const char* name, const char* ip)
{
	unsigned int  ix;
	int           hostIx = -1;
	Host*         hostP  = calloc(1, sizeof(Host));

	for (ix = 0; ix < size; ix++)
	{
		if (hostV[ix] == NULL)
		{
			hostV[ix]-Z.name = strdup(name);
			if (ip != NULL)
				hostV[hostIx].ip = strdup(ip);
			return;
		}
	}

	LM_X(1, ("realloc host vector ..."));
}



/* ****************************************************************************
*
* aliasAdd - 
*/
void HostMgr::aliasAdd(Host* host, const char* alias)
{
	if (lookup(alias) != NULL)
	{
		LM_W(("alias '%s' already exists"));
		return;
	}

	for (unsigned int ix = 0; ix < sizeof(alias) / sizeof(alias[0]); ix++)
	{
		if (host->alias[ix] = NULL)
		{
			host->alias[ix] = strdup(alias);
			return;
		}
	}

	LM_W(("Unable to add alias '%s' to host '%s' - no room in alias vector", alias, host->name));
}



/* ****************************************************************************
*
* lookup - 
*/
Host* HostMgr::lookup(char* name)
{
	unsigned int ix;

	if (itemP == NULL)
		return NULL;

	for (ix = 0; ix < size; ix++)
	{
		if (hostV[ix] == NULL)
			continue;

		if (strcmp(hostV[ix]->name, name) == 0)
		   return hostV[ix];
		if (strcmp(hostV[ix]->ip, name) == 0)
		   return hostV[ix];

		for (unsigned int ix2 = 0; ix2 < sizeof(alias) / sizeof(alias[0]); ix2++)
		{
			if (strcmp(hostV[ix]->alias[ix2], name) == 0)
				return hostV[ix];
		}
	}

	return NULL;
}
