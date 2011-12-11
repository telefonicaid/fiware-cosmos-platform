/* ****************************************************************************
*
* FILE                     WebWorkerEndpoint.cpp
*
* DESCRIPTION              Class for web worker endpoints
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 28 2011
*
*/
#include <unistd.h>             // read
#include <string>               // std::string

#include "samson/network/EndpointManager.h"    // EndpointManager
#include "WebWorkerEndpoint.h"  // Own interface



namespace samson
{



/* ****************************************************************************
*
* WebWorkerEndpoint - 
*/
WebWorkerEndpoint::WebWorkerEndpoint
(
	EndpointManager*  _epMgr,
	int               _id,
	Host*             _host,
	unsigned short    _port,
	int               _rFd,
	int               _wFd
) : Endpoint2(_epMgr, WebWorker, _id, _host, _port, _rFd, _wFd)
{
}



/* ****************************************************************************
*
* ~WebWorkerEndpoint - 
*/
WebWorkerEndpoint::~WebWorkerEndpoint() // : ~Endpoint2()
{
}



/* ****************************************************************************
*
* msgTreat
*/
Status WebWorkerEndpoint::msgTreat2(void)
{
	char buf[1024];
	int  nb;

	nb = read(rFd, buf, sizeof(buf));
	if (nb == -1)
	{
		msgsInErrors += 1;
		LM_RE(ReadError, ("error reading web service request: %s", strerror(errno)));
	}

	if (nb == 0)
	{
		msgsInErrors += 1;
		LM_RE(ReadError, ("read ZERO bytes of web service request"));
	}

	ssize_t s;

	std::string command     = epMgr->packetReceiver->getJSONStatus(std::string(buf));
	int         commandLen  = command.size();

	msgsIn  += 1;
	msgsOut += 1;
	s = write(wFd, command.c_str(), commandLen);
	if (s != commandLen)
		LM_W(("written only %d bytes (wanted to write %d)", s, commandLen));

	state = ScheduledForRemoval;

	return OK;
}



/* ****************************************************************************
*
* run - 
*/
void WebWorkerEndpoint::run()
{
	msgTreat();
}

}