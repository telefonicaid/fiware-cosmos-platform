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
#include "traceLevels.h"        // Trace Levels

#include "Endpoint.h"           // Endpoint
#include "iomMsgAwait.h"        // Own interface



/* ****************************************************************************
*
* iomMsgAwait - await a message from an endpoint
*/
int iomMsgAwait(int fd, int secs, int usecs)
{
	struct timeval  tv;
	struct timeval* tvP;
	int             fds;
	fd_set          rFds;

	if (secs == -1)
		tvP = NULL;
	else
	{
		tv.tv_sec  = secs;
		tv.tv_usec = usecs;

		tvP        = &tv;
	}

	do
	{
		FD_ZERO(&rFds);
		FD_SET(fd, &rFds);
		fds = select(fd + 1, &rFds, NULL, NULL, tvP);
	} while ((fds == -1) && (errno == EINTR));

	if (fds == -1)
		LM_RP(-1, ("select"));
	else if (fds == 0)
		LM_RE(-2, ("timeout"));
	else if ((fds > 0) && (!FD_ISSET(fd, &rFds)))
		LM_X(1, ("some other fd has a read pending - this is impossible !"));
	else if ((fds > 0) && (FD_ISSET(fd, &rFds)))
		return 1;

	LM_X(1, ("Other very strange error"));
	return -1;
}	
