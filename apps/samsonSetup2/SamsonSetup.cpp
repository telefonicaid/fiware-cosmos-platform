/* ****************************************************************************
*
* FILE                     SamsonSetup.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 04 2011
*
*/
#include "Network2.h"           // Network2
#include "Process.h"            // Process, ProcessVector
#include "ports.h"              // CONTROLLER_PORT, WORKER_PORT
#include "SamsonSetup.h"        // Own interface



namespace ss
{


/* ****************************************************************************
*
* SamsonSetup constructor - 
*/
SamsonSetup::SamsonSetup()
{
	EndpointManager* epMgr = new EndpointManager(Endpoint2::Setup);
	networkP               = new Network2(epMgr);
}



/* ****************************************************************************
*
* procVecCreate - 
*/
void SamsonSetup::procVecCreate(const char* controllerHost, int workers, const char* ips[])
{
	int            size = sizeof(ProcessVector) + workers * sizeof(Process) + sizeof(Process);
	Host*          hostP;
	ProcessVector* pv;
	Process*       p;
	Endpoint2*     ep;
	int            spawnerId = 0;

	if ((long) ips[0] != workers)
		LM_X(1, ("%d workers specified on command line, but %d ips in ip-list", workers, (long) ips[0]));

	LM_M(("-----------------------------------------------------------------------------------------------"));
	LM_M(("Creating a Process Vector of %d workers and a controller (in %s)", workers, controllerHost));

	pv = (ProcessVector*) calloc(1, size);
	if (pv == NULL)
		LM_X(1, ("error allocating %d bytes for process vector", size));
	pv->processes      = workers + 1;
	pv->processVecSize = size;

	for (int ix = 0; ix < pv->processes; ix++)
	{
		p = &pv->processV[ix];

		if (ix == 0) // Controller
		{
			snprintf(p->name,        sizeof(p->name),  "Controller");
			snprintf(p->alias,       sizeof(p->alias), "Controller");
			snprintf(p->host,        sizeof(p->host),   "%s", controllerHost);

			p->port = CONTROLLER_PORT;
			p->type = PtController;
		}
		else // Worker
		{
			snprintf(p->name,        sizeof(p->name),  "Worker");
			snprintf(p->alias,       sizeof(p->alias), "Worker%02d", ix - 1);
			snprintf(p->host,        sizeof(p->host),   "%s", ips[ix]);

			p->port = CONTROLLER_PORT;
			p->type = PtWorker;
		}

		hostP = networkP->epMgr->hostMgr->lookup(p->host);
		if (hostP == NULL)
		{
			LM_M(("Controller host '%s' not found in host manager - adding it", p->host));
			hostP = networkP->epMgr->hostMgr->insert(p->host, NULL);
			if (hostP == NULL)
				LM_X(1, ("error adding host '%s'", p->host));
		}

		snprintf(p->host, sizeof(p->host), "%s", hostP->name);
		snprintf(p->traceLevels, sizeof(p->traceLevels), "0");

		p->state     = Endpoint2::Unused;
		p->verbose   = false;
		p->debug     = false;
		p->reads     = false;
		p->writes    = false;
		p->hidden    = false;
		p->toDo      = false;

		LM_M(("Created Process for %s@%s", p->alias, hostP->name));

		if (networkP->epMgr->lookup(Endpoint2::Spawner, hostP->name) == NULL)
		{
			if ((ep = networkP->epMgr->add(Endpoint2::Spawner, spawnerId, "Spawner", "Spawner", hostP, SPAWNER_PORT)) != NULL)
				LM_M(("Created Spawner %d Endpoint for host %s", spawnerId, hostP->name));
			else
				LM_X(1, ("Error creating Spawner Endpoint for host %s", hostP->name));

			++spawnerId;
		}
		else
			LM_M(("Spawner endpoint for %s already created", hostP->name));

		LM_M(("---------------------------------------------------------------"));
	}

	LM_M(("Setting Endpoint Manager process vector"));
	networkP->epMgr->procVecSet(pv);
	spawners = spawnerId;
}



/* ****************************************************************************
*
* connect - 
*/
Endpoint2::Status SamsonSetup::connect(void)
{
	Endpoint2*        ep;
	Endpoint2::Status s;

	for (int ix = 0; ix < spawners; ix++)
	{
		ep = networkP->epMgr->lookup(Endpoint2::Spawner, ix);
		if (ep == NULL)
			LM_X(1, ("Cannot find Spawner %d", ix));

		LM_M(("Connecting to endpoint %s@%s", ep->nameGet(), ep->hostname()));
		if ((s = ep->connect()) != Endpoint2::OK)
			LM_RE(s, ("Error connecting to %s %d in %s: %s", ep->nameGet(), ep->idGet(), ep->hostname(), ep->status(s)));
	}

	return Endpoint2::OK;
}



/* ****************************************************************************
*
* reset - 
*/
Endpoint2::Status SamsonSetup::reset(void)
{
	Endpoint2*         ep;
	Endpoint2::Status  s;

	for (int ix = 0; ix < spawners; ix++)
	{
		ep = networkP->epMgr->lookup(Endpoint2::Spawner, ix);
		if (ep == NULL)
			LM_X(1, ("Cannot find Spawner %d", ix));

		LM_M(("Sending RESET to endpoint %s@%s", ep->nameGet(), ep->hostname()));
		if ((s = ep->send(Message::Msg, Message::Reset)) != Endpoint2::OK)
			LM_RE(s, ("send: %s", ep->status(s)));
	}

	return Endpoint2::OK;
}



/* ****************************************************************************
*
* processList - 
*/
Endpoint2::Status SamsonSetup::processList(void)
{
	return Endpoint2::Error;
}



/* ****************************************************************************
*
* procVecSend - 
*/
Endpoint2::Status SamsonSetup::procVecSend()
{
	Endpoint2*         ep;
	Endpoint2::Status  s;

	for (int ix = 0; ix < spawners; ix++)
	{
		ep = networkP->epMgr->lookup(Endpoint2::Spawner, ix);
		if (ep == NULL)
			LM_X(1, ("Cannot find Spawner %d", ix));

		LM_M(("Sending Process Vector to endpoint %s@%s", ep->nameGet(), ep->hostname()));
		if ((s = ep->send(Message::Msg, Message::Reset, networkP->epMgr->procVecGet(), networkP->epMgr->procVecGet()->processVecSize)) != Endpoint2::OK)
			LM_RE(s, ("send: %s", ep->status(s)));
	}

	return Endpoint2::OK;
}



/* ****************************************************************************
*
* run - 
*/
void SamsonSetup::run(void)
{
	networkP->run();
}

}
