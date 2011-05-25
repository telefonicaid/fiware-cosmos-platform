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

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // Trace Levels

#include "iomAccept.h"          // Own interface



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


extern int h_errno;
/* ****************************************************************************
*
* iomAccept -  worker accept
*/
int iomAccept(int lfd, struct sockaddr_in* sinP, char* hostName, int hostNameLen, char* ip, int ipLen)
{
	// struct hostent*     hP;
	unsigned int        len;
	int                 fd;

	memset((char*) sinP, 0, sizeof(struct sockaddr_in));
	len = sizeof(struct sockaddr_in);

	if ((fd = accept(lfd, (struct sockaddr*) sinP, &len)) == -1)
		LM_RP(-1, ("accept"));

	// To get hostname:  gethostbyaddr
	// To get IP:        inet_ntoa

	LM_T(LmtAccept, ("Accepted connection from 0x%x", sinP->sin_addr.s_addr));

	ip2string(sinP->sin_addr.s_addr, hostName, hostNameLen);

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

	// Disable the Nagle (TCP No Delay) algorithm
	int flag = 1;
	s = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(flag));
	if (s != 0)
		LM_X(1, ("setsockopt(TCP_NODELAY): %s", strerror(errno)));
#endif

	return fd;
}
