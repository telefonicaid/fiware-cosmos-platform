/* ****************************************************************************
*
* FILE                     iomConnect.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 11 2010
*
*/
#include <sys/types.h>          // types needed by socket include files
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <netinet/in.h>         // struct sockaddr_in
#include <netdb.h>              // gethostbyname
#include <arpa/inet.h>          // inet_ntoa

#include "logMsg.h"             // LM_*
#include "iomConnect.h"         // Own interface



/* ***************************************************************************
*
* iomConnect - 
*/
int iomConnect(const char* ip, unsigned short port)
{
	int                 fd;
	struct hostent*     hp;
	struct sockaddr_in  peer;

	if ((hp = gethostbyname(ip)) == NULL)
		LM_RP(-1, ("gethostbyname(%s)", ip));

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		LM_RE(-1, ("socket: %s", strerror(errno)));
	
	memset((char*) &peer, 0, sizeof(peer));

	peer.sin_family      = AF_INET;
	peer.sin_addr.s_addr = ((struct in_addr*) (hp->h_addr))->s_addr;
	peer.sin_port        = htons(port);

	LM_M(("calling connect"));
	if (connect(fd, (struct sockaddr*) &peer, sizeof(peer)) == -1)
	{
		LM_M(("connect: %s", strerror(errno)));
		close(fd);
		return -1;
	}

	int bufSize = 64 * 1024 * 1024;
	int s;
	s = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufSize, sizeof(bufSize));
	if (s != 0)
		LM_E(("setsockopt: %s", strerror(errno)));
	s = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bufSize, sizeof(bufSize));
	if (s != 0)
		LM_E(("setsockopt: %s", strerror(errno)));

	LM_M(("connect OK, returning fd %d", fd));

	return fd;
}
