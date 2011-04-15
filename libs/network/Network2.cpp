/* ****************************************************************************
*
* FILE                     Network.cpp - Definition for the network interface
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 08 2011
*
*/
#include <sys/select.h>         // select

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // Trace Levels

#include "iomServerOpen.h"      // iomServerOpen
#include "iomMsgSend.h"         // iomMsgSend
#include "Host.h"               // Host
#include "HostMgr.h"            // HostMgr
#include "Endpoint2.h"          // Endpoint2
#include "EndpointManager.h"    // EndpointManager
#include "Network2.h"           // Own interface



namespace ss
{



/* ****************************************************************************
*
* Constructor
*/
Network2::Network2(EndpointManager* _epMgr)
{
	epMgr     = _epMgr;
	tmoSecs   = 2;
	tmoUSecs  = 0;
}



/* ****************************************************************************
*
* tmoSet - 
*/
void Network2::tmoSet(int secs, int usecs)
{
	tmoSecs   = secs;
	tmoUSecs  = usecs;
}



/* ****************************************************************************
*
* run
*/
void Network2::run(bool oneShot)
{
	Endpoint2*       ep;
	int              ix;
	fd_set           rFds;
	int              rFd;
	int              max;
	struct timeval   tv;
	int              fds;

	while (1)
	{
		// Populate rFds for select
		do
		{
			FD_ZERO(&rFds);
			ix   = 0;
			max  = 0;
			while ((ep = epMgr->get(ix++, &rFd)) != NULL)
			{
				if (ep->stateGet() != Endpoint2::Ready)
					continue;

				max = MAX(max, rFd);
				FD_SET(rFd, &rFds);
			}

			tv.tv_sec  = tmoSecs;
			tv.tv_usec = tmoUSecs;
			
			fds = select(max + 1,  &rFds, NULL, NULL, &tv);
		} while ((fds == -1) && (errno == EINTR));

		if (fds == -1)
			LM_X(1, ("select: %s", strerror(errno)));
		else if (fds == 0)
		{
		}
		else
		{
			// Treat messages
			ix = 0;
			while ((ep = epMgr->get(ix++, &rFd)) != NULL)
			{
				if (FD_ISSET(rFd, &rFds))
					ep->msgTreat();
			}
		}

		if (oneShot)
			return;
	}
}

}
