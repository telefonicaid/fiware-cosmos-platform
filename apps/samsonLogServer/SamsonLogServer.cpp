/* ****************************************************************************
*
* FILE                     SamsonLogServer.cpp - 
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Jan 23 2011
*
*/
#include <sys/time.h>           // getimeofday

#include <QObject>
#include <QTimerEvent>

#include "logMsg.h"             // LM_*

#include "globals.h"            // networkP
#include "Network.h"            // Network
#include "Message.h"            // ss::Message::Header
#include "Popup.h"              // Popup windows
#include "logProviderList.h"    // logProviderAdd
#include "SamsonLogServer.h"    // Own interface



/* ****************************************************************************
*
* MAX - 
*/
#ifndef MAX
#define MAX(a, b)  ( ((a) > (b))? (a) : (b))
#endif



/* ****************************************************************************
*
* SamsonLogServer::SamsonLogServer - 
*/
SamsonLogServer::SamsonLogServer(void)
{
	startTimer(10);  // 10 millisecond timer
}



/* ****************************************************************************
*
* SamsonLogServer::timerEvent - 
*/
void SamsonLogServer::timerEvent(QTimerEvent* e)
{
	static struct timeval  lastShow = { 0, 0 };
	struct timeval         now;

	gettimeofday(&now, NULL);
	if (now.tv_sec - lastShow.tv_sec > 3)
	{
		if (networkP != NULL)
			networkP->endpointListShow("periodic");

		lastShow = now;
	}

	if (networkP != NULL)
		networkP->poll();
}



/* ****************************************************************************
*
* SamsonLogServer::ready - 
*/
int SamsonLogServer::ready(const char* info)
{
	LM_M(("Network ready - so what ?"));

	return 0;
}



/* ****************************************************************************
*
* SamsonLogServer::endpointUpdate
*/
int SamsonLogServer::endpointUpdate(ss::Endpoint* ep, ss::Endpoint::UpdateReason reason, const char* reasonText, void* info)
{
	ss::Endpoint*  newEp = (ss::Endpoint*) info;
	LogProvider*   lpP;

	if (reason == ss::Endpoint::SelectToBeCalled)
	{
		networkP->endpointListShow("endpointUpdate");
		return 0;
	}

	if ((reason == ss::Endpoint::NoLongerTemporal) && (newEp != NULL))
		LM_M(("********************* Got an Update Notification ('%s') for endpoint %p '%s' at '%s' (new ep: '%s' at '%s')", reasonText, ep, ep->name.c_str(), ep->ip.c_str(), newEp->name.c_str(), newEp->ip.c_str()));
	else if (ep != NULL)
		LM_M(("********************* Got an Update Notification ('%s') for endpoint %p '%s' at '%s'", reasonText, ep, ep->name.c_str(), ep->ip.c_str()));
	else
		LM_M(("********************* Got an Update Notification ('%s') for NULL endpoint", reasonText));

	networkP->endpointListShow("In endpointUpdate");

	lpP = logProviderLookup(ep);
	if (lpP == NULL)
		LM_E(("Cannot find log provider for endpoint %p", ep));

	switch (reason)
	{
	case ss::Endpoint::WorkerRemoved:
	case ss::Endpoint::ControllerRemoved:
	case ss::Endpoint::EndpointRemoved:
		if (lpP)
			logProviderStateSet(lpP, "disconnected");
		break;

	case ss::Endpoint::NoLongerTemporal:
		if (lpP)
		{
			LM_M(("Changing ep for '%s' from %p to %p", lpP->name, lpP->endpoint, newEp));
			lpP->endpoint = newEp;
			logProviderNameSet(lpP, newEp->name.c_str(), newEp->ip.c_str());
		}
		break;

	case ss::Endpoint::HelloReceived:
		LM_M(("HelloReceived: lpP at %p for endpoint '%s' at '%s'", lpP, ep->name.c_str(), ep->ip.c_str()));
		if (lpP == NULL)
			logProviderAdd(ep, ep->name.c_str(), ep->ip.c_str(), ep->rFd);
		else
			logProviderNameSet(lpP, ep->name.c_str(), ep->ip.c_str());
		break;

	default:
		LM_W(("Got an '%s' Update Notification and no action taken ...", reasonText));
		break;
	}

	return 0;
}



/* ****************************************************************************
*
* SamsonLogServer::receive
*/
int SamsonLogServer::receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP)
{
	ss::Endpoint* ep = networkP->endpointLookup(fromId);

	if (ep == NULL)
		LM_RE(0, ("Cannot find endpoint with id %d", fromId));

	if (ep->helloReceived != true)
		LM_RE(0, ("no hello received for endpoint '%s' at '%s', type '%s', state '%s'", ep->name.c_str(), ep->ip.c_str(), ep->typeName(), ep->stateName()));

	if (headerP->code == ss::Message::LogLine)
	{
		char                       line[1024];
		ss::Message::LogLineData*  logLine;
		LogProvider*               lpP;

		lpP = logProviderLookup(ep);
		if (lpP == NULL)
		{
			char why[256];

			logLine = (ss::Message::LogLineData*) dataP;

			LM_W(("LOG LINE: %c: %s[%d]:%s: %s", logLine->type, logLine->file, logLine->lineNo, logLine->fName, logLine->text));
			snprintf(why, sizeof(why), "Cannot find log provider for endpoint '%s' type '%s' %p", ep->name.c_str(), ep->typeName(), ep);
			networkP->endpointListShow(why);
			// LM_RE(0, ("Cannot find log provider for endpoint %p", ep));
			LM_X(123, ("Cannot find log provider for endpoint %p", ep));
		}

		logLine = (ss::Message::LogLineData*) dataP;

		if (headerP->type != ss::Message::Msg)
			LM_X(1, ("bad type of message: 0x%x", headerP->type));

		snprintf(line, sizeof(line), "%c: %s[%d]:%s: %s", logLine->type, logLine->file, logLine->lineNo, logLine->fName, logLine->text);
		logLine = (ss::Message::LogLineData*) dataP;

		lpP->list->addItem(QString(line));
		lpP->list->scrollToBottom();

		if ((logLine->type == 'E') || (logLine->type == 'X'))
		{
			char title[128];
			char line[512];

			if (logLine->type == 'W')
				snprintf(title, sizeof(title), "Warning");
			else if (logLine->type == 'E')
				snprintf(title, sizeof(title), "Error");
			else if (logLine->type == 'X')
				snprintf(title, sizeof(title), "Fatal Error");

			snprintf(line, sizeof(line), "%c: %s", logLine->type, logLine->text);
			new Popup(title, line);
		}
	}
	else
		LM_X(1, ("Don't know how to treat '%s' message", ss::Message::messageCode(headerP->code)));

	return 0;
}
