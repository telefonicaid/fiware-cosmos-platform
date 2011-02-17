/* ****************************************************************************
*
* FILE                     HostMgr.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Feb 10 2011
*
*/
#include <stdio.h>              // popen
#include <unistd.h>             // gethostname
#include <arpa/inet.h>          // sockaddr_in, inet_ntop
#include <ifaddrs.h>            // getifaddrs
#include <net/if.h>             // IFF_UP
#include <netdb.h>              // 

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "Host.h"               // Host
#include "HostMgr.h"            // Own interface
#include <stdlib.h>             // free



namespace ss
{



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

	localIps();

	list("At creation");
}



/* ****************************************************************************
*
* ipsGet - get all IPs for a machine
*/
void HostMgr::ipsGet(Host* hostP)
{
	struct ifaddrs*      addrs;
	struct ifaddrs*      iap;
	struct sockaddr_in*  sa;
	char                 buf[64];

	getifaddrs(&addrs);
	for (iap = addrs; iap != NULL; iap = iap->ifa_next)
	{
		if (iap->ifa_addr && (iap->ifa_flags & IFF_UP) && iap->ifa_addr->sa_family == AF_INET)
		{
			sa = (struct sockaddr_in*)(iap->ifa_addr);
			inet_ntop(iap->ifa_addr->sa_family, (void*) &(sa->sin_addr), buf, sizeof(buf));

			if ((hostP->ip == NULL) && (strcmp(buf, "127.0.0.1") != 0))
				hostP->ip = strdup(buf);
			else
				aliasAdd(hostP, buf);
		}
	}

	freeifaddrs(addrs);
}



/* ****************************************************************************
*
* localIps - 
*/
void HostMgr::localIps(void)
{
	char   hostName[128];
	char   domain[128];
	char   domainedName[128];
	Host*  hostP;

	if (gethostname(hostName, sizeof(hostName)) == -1)
		LM_X(1, ("gethostname: %s", strerror(errno)));

	hostP = insert(hostName, NULL);

	memset(domainedName, 0, sizeof(domainedName));
	if (getdomainname(domain, sizeof(domain)) == -1)
		LM_X(1, ("getdomainname: %s", strerror(errno)));

	LM_TODO(("Would gethostname ever returned the 'domained' name ?"));

	if (domainedName[0] != 0)
	{
		snprintf(domainedName, sizeof(domainedName), "%s.%s", hostName, domain);
		aliasAdd(hostP, domainedName);
	}
	else
		LM_W(("No domain ..."));

	aliasAdd(hostP, "localhost");
	aliasAdd(hostP, "127.0.0.1");

	ipsGet(hostP);
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
Host* HostMgr::insert(Host* hostP)
{
	unsigned int  ix;

	for (ix = 0; ix < size; ix++)
	{
		if (hostV[ix] == NULL)
		{
			hostV[ix] = hostP;
			return hostV[ix];
		}
	}

	LM_X(1, ("realloc host vector (%d hosts not enough) ...", size));
	return NULL;
}



/* ****************************************************************************
*
* HostMgr::insert - 
*/
Host* HostMgr::insert(const char* name, const char* ip)
{
	Host* hostP  = (Host*) calloc(1, sizeof(Host));

	if (hostP == NULL)
		LM_X(1, ("malloc(%d): %s", sizeof(Host), strerror(errno)));

	if (name == NULL)
		LM_X(1, ("NULL name - bad parameter"));

	hostP->name = strdup(name);
	if (ip != NULL)
		hostP->ip = strdup(ip);

	return insert(hostP);
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
		if (host->alias[ix] == NULL)
		{
			host->alias[ix] = strdup(alias);
			list("Just added an alias");
			return;
		}
	}

	LM_W(("Unable to add alias '%s' to host '%s' - no room in alias vector", alias, host->name));
}



/* ****************************************************************************
*
* lookup - 
*/
Host* HostMgr::lookup(const char* name)
{
	unsigned int ix;

	if (name == NULL)
		return NULL;

	for (ix = 0; ix < size; ix++)
	{
		if (hostV[ix] == NULL)
			continue;

		if ((hostV[ix]->name != NULL) && (strcmp(hostV[ix]->name, name) == 0))
		   return hostV[ix];

		if ((hostV[ix]->ip != NULL) && (strcmp(hostV[ix]->ip, name) == 0))
		   return hostV[ix];

		for (unsigned int aIx = 0; aIx < sizeof(hostV[ix]->alias) / sizeof(hostV[ix]->alias[0]); aIx++)
		{
			if (hostV[ix]->alias[aIx] == NULL)
				continue;

			if (strcmp(hostV[ix]->alias[aIx], name) == 0)
				return hostV[ix];
		}
	}

	return NULL;
}



/* ****************************************************************************
*
* list - list the hosts in the list
*/
void HostMgr::list(const char* why)
{
	unsigned int ix;

	LM_F(("------------ Host List: %s ------------", why));
	for (ix = 0; ix < size; ix++)
    {
		char  line[512];
		Host* hostP;

        if (hostV[ix] == NULL)
            continue;

		hostP = hostV[ix];
		memset(line, sizeof(line), 0);
		snprintf(line, sizeof(line), "%02d: %-20s %-20s", ix, hostP->name, hostP->ip);

		for (unsigned int aIx = 0; aIx < sizeof(hostP->alias) / sizeof(hostP->alias[0]); aIx++)
		{
			char part[64];
			if (hostP->alias[aIx] == NULL)
				continue;

			snprintf(part, sizeof(part), " %-20s", hostP->alias[aIx]);
			strncat(line, part, sizeof(line));
		}

		LM_F((line));
	}
}


}
