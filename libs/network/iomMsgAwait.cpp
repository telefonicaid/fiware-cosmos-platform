/* ****************************************************************************
*
* FILE                     iomMsgAwait.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Oct 13 2010
*
*/
#include <sys/time.h>           // struct timeval
#include <sys/types.h>          // fd_set
#include <sys/select.h>         // select, ...

#include "logMsg.h"             // LM_*
#include "networkTraceLevels.h" // LMT_NWRUN, ...

#include "Endpoint.h"           // Endpoint
#include "iomMsgAwait.h"        // Own interface



/* ****************************************************************************
*
* iomMsgAwait - await a message from an endpoint
*/
int iomMsgAwait(ss::Endpoint* ep, int secs, int usecs)
{
	struct timeval tv;
	int            fds;
	fd_set         rFds;

	tv.tv_sec  = secs;
	tv.tv_usec = usecs;

	do
	{
		FD_ZERO(&rFds);
		FD_SET(ep->fd, &rFds);
		fds = select(ep->fd + 1, &rFds, NULL, NULL, &tv);
	} while ((fds == -1) && (errno == EINTR));

	if (fds == -1)
		LM_RP(-1, ("select"));
	else if (fds == 0)
		LM_RE(-1, ("timeout"));
	else if ((fds > 0) && (!FD_ISSET(ep->fd, &rFds)))
		LM_X(1, ("some other fd has a read pending - this is impossible !"));
	else if ((fds > 0) && (FD_ISSET(ep->fd, &rFds)))
		return 0;

	LM_X(1, ("Other very strange error"));
	return -1;
}	
