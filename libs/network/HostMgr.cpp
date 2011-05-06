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
#include <string.h>             // strstr

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
	// list("init");
}



/* ****************************************************************************
*
* ~HostMgr
*/
HostMgr::~HostMgr()
{
	unsigned int  ix;

	for (ix = 0; ix < size; ix++)
	{
		if (hostV[ix] == NULL)
			continue;

		remove(hostV[ix]->name);
	}
	
	free(hostV);
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
			{
				hostP->ip = strdup(buf);
			}
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

	if (gethostname(hostName, sizeof(hostName)) == -1)
		LM_X(1, ("gethostname: %s", strerror(errno)));

	localhostP = insert(hostName, "127.0.0.1"); // , "127.0.0.1" ...

	memset(domainedName, 0, sizeof(domainedName));
	if (getdomainname(domain, sizeof(domain)) == -1)
		LM_X(1, ("getdomainname: %s", strerror(errno)));

	LM_TODO(("Would gethostname ever returned the 'domained' name ?"));

	if (domainedName[0] != 0)
	{
		snprintf(domainedName, sizeof(domainedName), "%s.%s", hostName, domain);
		aliasAdd(localhostP, domainedName);
	}

	aliasAdd(localhostP, "localhost");

	ipsGet(localhostP);
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
			list("Host Added");
			return hostV[ix];
		}
	}

	LM_RE(NULL, ("Please realloc host vector (%d hosts not enough) ...", size));
}



/* ****************************************************************************
*
* ip2string - convert integer ip address to string
*/
static void ip2string(int ip, char* ipString, int ipStringLen)
{
	snprintf(ipString, ipStringLen, "%d.%d.%d.%d",
			 ip & 0xFF,
			 (ip & 0xFF00) >> 8,
			 (ip & 0xFF0000) >> 16,
			 (ip & 0xFF000000) >> 24);
}



/* ****************************************************************************
*
* onlyDigitsAndDots - 
*/
static bool onlyDigitsAndDots(const char* string)
{
	if ((string == NULL) || (string[0] == 0))
		LM_RE(false, ("Empty IP ..."));

	for (unsigned int ix = 0; ix < strlen(string); ix++)
	{
		if (string[ix] == 0)
			return true;

		if (string[ix] == '.')
			continue;

		if ((string[ix] >= '0') && (string[ix] <= '9'))
			continue;

		return false;
	}

	return true;
}



/* ****************************************************************************
*
* HostMgr::insert - 
*/
Host* HostMgr::insert(const char* name, const char* ip)
{
	Host*  hostP;
	char   ipX[64];
	char*  dotP;
	char*  alias = NULL;

	if ((name == NULL) && (ip == NULL))
		LM_X(1, ("name AND ip NULL - cannot add a ghost host ..."));

	if (name == NULL)
	{
		LM_W(("NULL host name for IP '%s'", ip));
		name = ip;
	}

	if ((hostP = lookup(name)) != NULL)
	{
		LM_D(("host '%s' already in host list", name));
		return hostP;
	}

	struct hostent* heP;

	heP = gethostbyname(name);
	
	if (heP == NULL)
		LM_W(("gethostbyname(%s) error", name));
	else
	{
		int ix = 0;

		ip2string(*((int*) heP->h_addr_list[ix]), ipX, sizeof(ipX));
		if (ip == NULL)
			ip = ipX; 
		else
			alias = ipX;

		name = heP->h_name;

		while (heP->h_aliases[ix] != NULL)
		{
			LM_TODO(("alias %d: '%s' - should also be added?", ix, heP->h_aliases[ix]));
			++ix;
		}

		for (ix = 1; ix < heP->h_length / 4; ix++)
		{
			if (heP->h_addr_list[ix] != NULL)
			{
				char ipY[64];

				ip2string(*((int*) heP->h_addr_list[ix]), ipX, sizeof(ipX));
				LM_TODO(("addr %d: '%s' should also be added?", ix, ipY));
			}
		}
	}

	// Already present ?
	if ((ip != NULL) && (ip[0] != 0) && ((hostP = lookup(ip)) != NULL))
		return hostP;

	hostP  = (Host*) calloc(1, sizeof(Host));
	if (hostP == NULL)
		LM_X(1, ("malloc(%d): %s", sizeof(Host), strerror(errno)));

	if (name != NULL)
		hostP->name = strdup(name);

	if (ip != NULL)
		hostP->ip = strdup(ip);

	if (alias != NULL)
		aliasAdd(hostP, alias);

	if ((dotP = (char*) strstr(name, ".")) != NULL)
	{
		if (onlyDigitsAndDots(name) == false)
		{
			LM_M(("Adding name without dots as an alias: '%s'", name));
			*dotP = 0;
			LM_M(("Adding name without dots as an alias: '%s'", name));
			aliasAdd(hostP, name);
		}
	}

	if ((dotP = (char*) strstr(ip, ".")) != NULL)
	{
		if (onlyDigitsAndDots(ip) == false)
		{
			LM_M(("Adding name without dots as an alias: '%s'", ip));
			*dotP = 0;
			LM_M(("Adding name without dots as an alias: '%s'", ip));
			aliasAdd(hostP, ip);
		}
	}

	return insert(hostP);
}



/* ****************************************************************************
*
* aliasAdd - 
*/
void HostMgr::aliasAdd(Host* host, const char* alias)
{
	Host* hp;

	if (host == NULL)
		LM_RVE(("Host is NULL"));
	if (alias == NULL)
		LM_RVE(("Alias is NULL (host: '%s')", host->name));

	if ((hp = lookup(alias)) != NULL)
	{
		if (hp != host)
			LM_X(1, ("Trying to add alias '%s' for host '%s', but the host '%s' already responds to it ...", alias, host->name, hp->name));
		return;
	}

	for (unsigned int ix = 0; ix < sizeof(alias) / sizeof(alias[0]); ix++)
	{
		if (host->alias[ix] == NULL)
			continue;

		if (strcmp(host->alias[ix], alias) == 0)
		{
			LM_W(("alias '%s' already present for host '%s'", alias, host->name));
			return;
		}
	}

	for (unsigned int ix = 0; ix < sizeof(alias) / sizeof(alias[0]); ix++)
	{
		if (host->alias[ix] == NULL)
		{
			host->alias[ix] = strdup(alias);
			return;
		}
	}

	LM_W(("Unable to add alias '%s' to host '%s' - no room in alias vector", alias, host->name));
}



/* ****************************************************************************
*
* remove - 
*/
bool HostMgr::remove(const char* name)
{
	Host*          hostP;
	unsigned int   ix;

	hostP = lookup(name);
	if (hostP == NULL)
		LM_RE(false, ("Host '%s' not in list"));

	for (ix = 0; ix < size; ix++)
	{
		if (hostV[ix] != hostP)
			continue;

		if (hostP->name != NULL)
			free(hostP->name);

		if (hostP->ip != NULL)
			free(hostP->ip);

		for (unsigned int aIx = 0; aIx < sizeof(hostP->alias) / sizeof(hostP->alias[0]); aIx++)
		{
			if (hostP->alias[aIx] == NULL)
				continue;

			free(hostP->alias[aIx]);
		}

		free(hostP);
		hostV[ix] = NULL;

		return true;
	}

	LM_RE(false, ("host pointer reurned from lookup not found ... internal bug!"));
}



/* ****************************************************************************
*
* lookup - 
*/
Host* HostMgr::lookup(const char* name)
{
	unsigned int ix;

	if (name == NULL)
		LM_RE(NULL, ("Cannot lookup a NULL hostname!"));

	LM_M(("Looking up host '%s'", name));

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
	
	if (strstr(name, ".") != NULL)
	{
		char* newName;
		char* dot;
		Host* h = NULL;

		if (onlyDigitsAndDots(name) == true)
			return NULL;

		LM_W(("Host '%s' was not found and has a dot in it - possible domain name poking us - lets remove it and try again ...", name));
		newName = strdup(name);
		dot     = strstr(newName, ".");
		if (dot != NULL)  // I'm a bit paranoid, sorry.
		{
			*dot = 0;
			LM_M(("Recursive call, using hostname '%s' instead of '%s'", newName, name));
			h = lookup(newName);
		}

		free(newName);
		return h;
	}
	
	return NULL;
}



/* ****************************************************************************
*
* lookup - 
*/
bool HostMgr::match(Host* host, const char* ip)
{
	if (ip == NULL)
		return NULL;

	if (ip[0] == 0)
		return NULL;

	if ((host->name != NULL) && (strcmp(host->name, ip) == 0))
		return true;

	if ((host->ip != NULL) && (strcmp(host->ip, ip) == 0))
		return true;

	for (unsigned int aIx = 0; aIx < sizeof(host->alias) / sizeof(host->alias[0]); aIx++)
	{
		if (host->alias[aIx] == NULL)
			continue;

		if (strcmp(host->alias[aIx], ip) == 0)
			return true;
	}

	return false;
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
		memset(line, 0, sizeof(line));
		snprintf(line, sizeof(line), "%02d: %-20s %-20s", ix, hostP->name, hostP->ip);

		for (unsigned int aIx = 0; aIx < sizeof(hostP->alias) / sizeof(hostP->alias[0]); aIx++)
		{
			char part[64];
			if (hostP->alias[aIx] == NULL)
				continue;

			snprintf(part, sizeof(part), " %-20s", hostP->alias[aIx]);
			strncat(line, part, sizeof(line) - 1);
		}

		LM_F((line));
	}
	LM_F(("---------------------------------------------------"));
}


}
