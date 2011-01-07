/* ****************************************************************************
*
* FILE                     iomAccept.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 11 2010
*
*/
#include <sys/socket.h>         // AF_INET
#include <netinet/in.h>         // struct in_addr
#include <netdb.h>              // gethostbyaddr
#include <arpa/inet.h>          // inet_ntoa
#include <vector>               // vector

#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_*

#include "iomAccept.h"          // Own interface



/* ****************************************************************************
*
* iomAccept -  worker accept
*/
int iomAccept(int lfd, char* hostName, int hostNameLen)
{
	struct sockaddr_in  peer;
	unsigned int        len;
	struct hostent*     hP;
	char                ip[64];
	char*               hName;
	int                 fd;

	memset((char*) &peer, 0, sizeof(struct sockaddr_in));
	len = sizeof(struct sockaddr_in);

	if ((fd = accept(lfd, (struct sockaddr*) &peer, &len)) == -1)
		LM_RP(-1, ("accept"));

	hP = gethostbyaddr((char*) &peer.sin_addr, sizeof(int), AF_INET);
	if (hP != NULL)
		hName = hP->h_name;
	else
	{
		LM_W(("gethostbyaddr failed for '%s': %s", ip, strerror(errno)));
		sprintf(ip, "%s", inet_ntoa(peer.sin_addr));
		hName = ip;
	}

	LM_T(LMT_ACCEPT, ("Accepted connection from host '%s'", hName));

	if (hostName)
		strncpy(hostName, hName, hostNameLen);

	int bufSize = 64 * 1024 * 1024;
	int s;
	s = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufSize, sizeof(bufSize));
	if (s != 0)
		LM_E(("setsockopt: %s", strerror(errno)));
	s = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bufSize, sizeof(bufSize));
	if (s != 0)
		LM_E(("setsockopt: %s", strerror(errno)));

	return fd;
}
