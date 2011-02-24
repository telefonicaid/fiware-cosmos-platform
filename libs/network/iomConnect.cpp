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
#include <unistd.h>             // close
#include <stdlib.h>             // free
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <netinet/in.h>         // struct sockaddr_in
#include <netdb.h>              // gethostbyname
#include <arpa/inet.h>          // inet_ntoa
#include <netinet/tcp.h>        // TCP_NODELAY

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace levels
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
		return -1;

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		LM_RE(-1, ("socket: %s", strerror(errno)));
	
	memset((char*) &peer, 0, sizeof(peer));

	peer.sin_family      = AF_INET;
	peer.sin_addr.s_addr = ((struct in_addr*) (hp->h_addr))->s_addr;
	peer.sin_port        = htons(port);

	if (connect(fd, (struct sockaddr*) &peer, sizeof(peer)) == -1)
	{
		usleep(50000);
		if (connect(fd, (struct sockaddr*) &peer, sizeof(peer)) == -1)
		{
			close(fd);
			return -1;
		}
	}

#if 0
	int bufSize = 64 * 1024 * 1024;

	int s;
	s = setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &bufSize, sizeof(bufSize));
	if (s != 0)
		LM_X(1, ("setsockopt(SO_RCVBUF): %s", strerror(errno)));
	s = setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &bufSize, sizeof(bufSize));
	if (s != 0)
		LM_X(1, ("setsockopt(SO_SNDBUF): %s", strerror(errno)));
#endif

#if 0
	// Disable the Nagle (TCP No Delay) algorithm
	int flag = 1;
	s = setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (char*) &flag, sizeof(flag));
	if (s != 0)
		LM_X(1, ("setsockopt(TCP_NODELAY): %s", strerror(errno)));
#endif

	LM_T(LmtConnect, ("connected to '%s', port %d, returning fd %d", ip, port, fd));

	return fd;
}
