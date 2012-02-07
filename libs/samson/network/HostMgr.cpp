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

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Trace Levels

#include "samson/network/misc.h"

#include "Host.h"               // Host
#include "HostMgr.h"            // Own interface
#include <stdlib.h>             // free



namespace samson
{



/* ****************************************************************************
*
* HostMgr
*/
HostMgr::HostMgr()
{
	LM_T(LmtHost, ("Creating Host Manager"));


	localIps();
	list("localhost inserted", true);
}



/* ****************************************************************************
*
* ~HostMgr
*/
HostMgr::~HostMgr()
{
    hostV.clearVector();
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

	while (1)
	{
		if (gethostname(hostName, sizeof(hostName)) == -1)
			LM_X(1, ("gethostname: %s", strerror(errno)));

		if (strcmp(hostName, "localhost") == 0)
		{
			LM_W(("Host name 'localhost' - awaiting an allowed host name ..."));
			sleep(1);
			continue;
		}

		break;
	}

	LM_T(LmtHost, ("hostName: '%s'", hostName));
	localhostP = insert(hostName, "127.0.0.1"); // , "127.0.0.1" ...

	memset(domainedName, 0, sizeof(domainedName));
	if (getdomainname(domain, sizeof(domain)) == -1)
		LM_X(1, ("getdomainname: %s", strerror(errno)));

	LM_TODO(("Would gethostname ever return the 'domained' name ?"));

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
    return hostV.size();
}



/* ****************************************************************************
*
* HostMgr::insert - 
*/
Host* HostMgr::insert(Host* hostP)
{
    hostV.push_back(hostP);
    list("Host Added", true);
	return hostP;
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



static void newCheck(const char* name, const char* ip)
{
	int               s;
	char*             node = (char*) name;
    struct addrinfo*  result;
    struct addrinfo*  res;

	if (node == NULL)
		node = (char*) ip;

	s = getaddrinfo(node, NULL, NULL, &result);
	if (s == 0)
	{
		for (res = result; res != NULL; res = res->ai_next)
		{   
			char hostname[NI_MAXHOST] = "";
			
			s = getnameinfo(res->ai_addr, res->ai_addrlen, hostname, NI_MAXHOST, NULL, 0, 0); 
			if (s != 0)
			{
				LM_E(("error in getnameinfo: %s", gai_strerror(s)));
				continue;
			}
		}

		freeaddrinfo(result);
	}
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

	newCheck(name, ip);

	if (name == NULL)
	{
		LM_T(LmtHost, ("NULL host name for IP '%s'", ip));
		name = ip;
	}

	LM_T(LmtHost, ("inserting '%s'", name));
	if ((hostP = lookup(name)) != NULL)
	{
		LM_T(LmtHost, ("host '%s' already in host list", name));
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
	LM_T(LmtHost, ("Already present? '%s'", ip));
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

	if (name != NULL)
	{
		if ((dotP = (char*) strstr(name, ".")) != NULL)
		{
			if (onlyDigitsAndDots(name) == false)
			{
				LM_T(LmtHost, ("Adding name without dots as an alias: '%s'", name));
				*dotP = 0;
				LM_T(LmtHost, ("Adding name without dots as an alias: '%s'", name));
				aliasAdd(hostP, name);
			}
		}
	}

	if (ip != NULL)
	{
		if ((dotP = (char*) strstr(ip, ".")) != NULL)
		{
			if (onlyDigitsAndDots(ip) == false)
			{
				LM_T(LmtHost, ("Adding name without dots as an alias: '%s'", ip));
				*dotP = 0;
				LM_T(LmtHost, ("Adding name without dots as an alias: '%s'", ip));
				aliasAdd(hostP, ip);
			}
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

	LM_T(LmtHost, ("adding alias '%s'", alias));
	if ((hp = lookup(alias)) != NULL)
	{
		if (hp != host)
			LM_X(1, ("Trying to add alias '%s' for host '%s', but the host '%s' already responds to it ...", alias, host->name, hp->name));
		return;
	}

	for (unsigned int ix = 0; ix < sizeof(host->alias) / sizeof(host->alias[0]); ix++)
	{
		if (host->alias[ix] == NULL)
			continue;

		if (strcmp(host->alias[ix], alias) == 0)
		{
			LM_W(("alias '%s' already present for host '%s'", alias, host->name));
			return;
		}
	}

	for (unsigned int ix = 0; ix < sizeof(host->alias) / sizeof(host->alias[0]); ix++)
	{
		if (host->alias[ix] == NULL)
		{
			LM_T(LmtHost, ("Added alias %d '%s' for host '%s'", ix, alias, host->name));
			host->alias[ix] = strdup(alias);
			return;
		}
	}

	LM_W(("Unable to add alias '%s' to host '%s' - no room in alias vector (max slots %d)", alias, host->name, sizeof(alias) / sizeof(alias[0])));
}



/* ****************************************************************************
*
* remove - 
*/
bool HostMgr::remove(const char* name)
{
    Host* host = lookup(name);
    
    if( host )
    {
        hostV.removeFromVector( host );
        delete host;
        return true;
    }
    
    return false;
}



/* ****************************************************************************
*
* lookup - 
*/
Host* HostMgr::lookup(const char* name)
{
	LM_T(LmtHost, ("looking up '%s'", name));
 
	if (name == NULL)
		LM_RE(NULL, ("Cannot lookup a NULL hostname!"));

    for( size_t i = 0 ; i < hostV.size() ; i++ )
        if ( hostV[i]->match(name) )
            return hostV[i];
    
	if (strstr(name, ".") != NULL)
	{
		char* newName;
		char* dot;
		Host* h = NULL;

		if (onlyDigitsAndDots(name) == true)
			return NULL;

		LM_T(LmtHost, ("Host '%s' was not found and has a dot in it - possible domain name poking us - lets remove it and try again ...", name));
		newName = strdup(name);
		dot     = strstr(newName, ".");
		if (dot != NULL)  // I'm a bit paranoid, sorry.
		{
			*dot = 0;
			LM_T(LmtHost, ("Recursive call, using hostname '%s' instead of '%s'", newName, name));
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




/* ****************************************************************************
*
* list - list the hosts in the list
*/
void HostMgr::list(const char* why, bool forced)
{
	int           tLevel = LmtHost;

	LM_T(tLevel, ("------------ Host List: %s ------------", why));
    for ( size_t i = 0 ; i < hostV.size() ; i++ )
    {
		Host* hostP = hostV[i];
        
		char  line[512];

		memset(line, 0, sizeof(line));
		snprintf(line, sizeof(line), "%-20s %-20s", hostP->name, hostP->ip);

		for (unsigned int aIx = 0; aIx < sizeof(hostP->alias) / sizeof(hostP->alias[0]); aIx++)
		{
			char part[64];
			if (hostP->alias[aIx] == NULL)
				continue;

			snprintf(part, sizeof(part), " %-20s", hostP->alias[aIx]);
			strncat(line, part, sizeof(line) - 1);
		}

		LM_T(tLevel, (line));
	}
	LM_T(tLevel, ("---------------------------------------------------"));
}


}
