/* ****************************************************************************
*
* FILE                     Misc.h - miscellaneous functions
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Nov 04 2010
*
*/
#include <stdio.h>              // FILE, popen, fgets, ...
#include <cstdlib>				// free

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Trace Levels

#include "Misc.h"               // Own interface



#ifdef __linux__

#include <stropts.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/netdevice.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>



/* ****************************************************************************
*
* ipGet - 
*/
char* ipGet(void)
{
	int             s;
	struct ifconf   ifconf;
	struct ifreq    ifr[50];
	int             ifs;
	int             i;
	int             domain = AF_INET;

	s = socket(domain, SOCK_STREAM, 0);
	if (s < 0)
	{
		LM_E(("socket: %s", strerror(errno)));
		return strdup("noip");
	}

	ifconf.ifc_buf = (char *) ifr;
	ifconf.ifc_len = sizeof ifr;

	if (ioctl(s, SIOCGIFCONF, &ifconf) == -1)
	{
		LM_E(("ioctl(SIOCGIFCONF): %s", strerror(errno)));
		close(s);
		return strdup("noip");
	}

	ifs = ifconf.ifc_len / sizeof(ifr[0]);
	for (i = 0; i < ifs; i++)
	{
		char ip[INET_ADDRSTRLEN];
		struct sockaddr_in* s_in = (struct sockaddr_in*) &ifr[i].ifr_addr;

		if (!inet_ntop(domain, &s_in->sin_addr, ip, sizeof(ip)))
		{
			LM_E(("inet_ntop: %s", strerror(errno)));
			close(s);
			return strdup("noip");
		}

		if (strncmp(ifr[i].ifr_name, "eth", 3) == 0)
		{
			close(s);
			return strdup(ip);
		}
	}

	close(s);

	return strdup("noip");
}

#else

char* ipGet(void)
{
	char  line[80];
	char* ipP = (char*) "II.PP";
	FILE* fP;
	
	LM_T(LmtInit, ("Calling popen(ifconfig | grep \"inet addr:\" ...)"));
	fP = popen("ifconfig | grep \"inet addr:\" | awk -F: '{ print $2 }' | awk '{ print $1 }'", "r");
	LM_T(LmtInit, ("popen returned %p", fP));
	if (fgets(line, sizeof(line), fP) != NULL)
	{
		if (line[strlen(line) - 1] == '\n')
			line[strlen(line) - 1] = 0;
		ipP = wordClean(line);
		LM_T(LmtInit, ("new IP: %s", ipP));
	}

	fclose(fP);

	return strdup(ipP);
}
#endif



/* ****************************************************************************
*
* wordClean - 
*/
char* wordClean(char* str)
{
	char* endP;

	while ((*str == ' ') || (*str == '\t'))
		++str;

	endP = str;
	while ((*endP != 0) && (*endP != ' ') && (*endP != '\t'))
		++endP;
	*endP = 0;

	return str;
}
