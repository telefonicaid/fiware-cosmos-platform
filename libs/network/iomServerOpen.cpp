/* ****************************************************************************
*
* FILE                     iomServerOpen.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 11 2010
*
*/
#include <unistd.h>             // close
#include <memory.h>             // memset
#include <sys/types.h>          // types needed by socket include files
#include <sys/socket.h>         // socket, bind, listen
#include <sys/un.h>             // sockaddr_un
#include <netinet/in.h>         // struct sockaddr_in
#include <fcntl.h>              // F_SETFD
#include <cstdlib>              // free

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // LMT_NWRUN, ...

#include "iomServerOpen.h"      // Own interface



/* ****************************************************************************
*
* iomServerOpen - open a listen socket on the specified port
*/
int iomServerOpen(unsigned short port)
{
	int                 sFd;
	int                 reuse = 1;
	struct sockaddr_in  sock;
	struct sockaddr_in  peer;

	if ((sFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		LM_RP(-1, ("socket"));

	fcntl(sFd, F_SETFD, 1);

	memset((char*) &sock, 0, sizeof(sock));
	memset((char*) &peer, 0, sizeof(peer));

	sock.sin_family      = AF_INET;
	sock.sin_addr.s_addr = INADDR_ANY;
	sock.sin_port        = htons(port);
	
	setsockopt(sFd, SOL_SOCKET, SO_REUSEADDR, (char*) &reuse, sizeof(reuse));
	
	if (bind(sFd, (struct sockaddr*) &sock, sizeof(struct sockaddr_in)) == -1)
	{
		close(sFd);
		LM_RP(-1, ("bind to port %d: %s", port, strerror(errno)));
	}

	if (listen(sFd, 10) == -1)
	{
		close(sFd);
		LM_RP(-1, ("listen to port %d", port));
	}

	LM_T(LmtFds, ("Opened listening socket on fd %d for port %d", sFd, port));

	return sFd;
}
