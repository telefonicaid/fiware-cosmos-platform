/* ****************************************************************************
*
* FILE                     SamsonSupervisor.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Dec 15 2010
*
*/
#include "logMsg.h"             // LM_*

#include "globals.h"            // tabManager, ...
#include "NetworkInterface.h"   // DataReceiverInterface, EndpointUpdateInterface
#include "Endpoint.h"           // Endpoint
#include "Network.h"            // Network
#include "Message.h"            // ss::Message::Header
#include "actions.h"            // help, list, start, ...
#include "Starter.h"            // Starter
#include "SamsonSupervisor.h"   // Own interface



/* ****************************************************************************
*
* SamsonSupervisor::receive - 
*/
int SamsonSupervisor::receive(int fromId, int nb, ss::Message::Header* headerP, void* dataP)
{
	ss::Endpoint* ep = networkP->endpointLookup(fromId);

	if (ep == NULL)
		LM_RE(0, ("Cannot find endpoint with id %d", fromId));

	if (ep->type == ss::Endpoint::Fd)
	{
		char* msg = (char*) dataP;

		printf("\n");
		switch (*msg)
		{
		case 'h':
			help();
			break;

		case 'c':
			connectToAllSpawners();
			break;

		case 'p':
			startAllProcesses();
			break;

		case 's':
			start();
			break;

		case 'l':
			list();
			break;

		case 3:
			LM_X(0, ("'Ctrl-C' pressed - I quit!"));

		case 'q':
			LM_X(0, ("'q' pressed - I quit!"));

		case ' ':
		case '\n':
			printf("\n");
			break;

		default:
			LM_E(("Key '%c' has no function", *msg));
			help();
		}

		printf("\n");
		return 0;
	}

	switch (headerP->code)
	{
	case ss::Message::WorkerSpawn:
	case ss::Message::ControllerSpawn:

	default:
		LM_X(1, ("Don't know how to treat '%s' message", ss::Message::messageCode(headerP->code)));
	}

	return 0;
}



/* ****************************************************************************
*
* SamsonSupervisor::endpointUpdate - 
*/
int SamsonSupervisor::endpointUpdate(ss::Endpoint* ep, const char* reason, void* info)
{
	LM_M(("********************* Got an Update Notification ('%s') for endpoint '%s' at '%s'", reason, ep->name.c_str(), ep->ip.c_str()));

	Starter* starter;

	starter = tabManager->processListTab->starterLookup(ep);
	if ((starter == NULL) && (info != NULL))
	{
		starter = tabManager->processListTab->starterLookup((ss::Endpoint*) info);
		if (starter)
			starter->endpoint = (ss::Endpoint*) info;
		ep = (ss::Endpoint*) info;
	}

	if (starter == NULL)
	{
		LM_M(("********************* Cannot find starter for endpoint at %p (%p)", ep, info));
		return -1;
	}

	if (ep->state == ss::Endpoint::Connected)
	{
		LM_M(("********************* Setting state to Connected for endpoint '%s' at '%s'", ep->name.c_str(), ep->ip.c_str()));
		starter->checkbox->setCheckState(Qt::Checked);
	}
	else
	{
		LM_M(("********************* Setting state to Disconnected for endpoint '%s' at '%s'", ep->name.c_str(), ep->ip.c_str()));
		starter->checkbox->setCheckState(Qt::Unchecked);
	}

	return 0;
}
