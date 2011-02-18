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
#include <netinet/tcp.h>        // TCP_NODELAY
#include <vector>               // vector

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "iomAccept.h"          // Own interface


extern int h_errno;
/* ****************************************************************************
*
* iomAccept -  worker accept
*/
int iomAccept(int lfd, struct sockaddr_in* sinP, char* hostName, int hostNameLen, char* ip, int ipLen)
{
	unsigned int        len;
	struct hostent*     hP;
	int                 fd;

	memset((char*) sinP, 0, sizeof(struct sockaddr_in));
	len = sizeof(struct sockaddr_in);

	if ((fd = accept(lfd, (struct sockaddr*) sinP, &len)) == -1)
		LM_RP(-1, ("accept"));

	// To get hostname:  gethostbyaddr
	// To get IP:        inet_ntoa

	hP = gethostbyaddr((void*) &sinP->sin_addr, sizeof(int), AF_INET);
	if (hP != NULL)
	{
		if (hostName != NULL)
			strncpy(hostName, hP->h_name, hostNameLen);
	}
	else
		LM_E(("gethostbyaddr failed for '0x%x': %s. h_errno: %d", sinP->sin_addr.s_addr, strerror(errno), h_errno));

	if (ip)
		strncpy(ip, inet_ntoa(sinP->sin_addr), ipLen);

	LM_T(LmtAccept, ("Accepted connection from host '%s'", (hostName != NULL)? hostName : inet_ntoa(sinP->sin_addr)));
		
#if 0
Andreu: 
  This crashes samsonWorker ...
  Deactivating, at least temporary

	int bufSize = 64 * 1024 * 1024;
	int s;

	s = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufSize, sizeof(bufSize));
	if (s != 0)
		LM_X(1, ("setsockopt(SO_RCVBUF): %s", strerror(errno)));
	s = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bufSize, sizeof(bufSize));
	if (s != 0)
		LM_X(1, ("setsockopt(SO_SNDBUF): %s", strerror(errno)));

#if 0
	// Disable the Nagle (TCP No Delay) algorithm
	int flag = 1;
	s = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(flag));
	if (s != 0)
		LM_X(1, ("setsockopt(TCP_NODELAY): %s", strerror(errno)));
#endif
#endif

	return fd;
}
