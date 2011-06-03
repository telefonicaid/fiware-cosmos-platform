/* ****************************************************************************
*
* FILE                     SamsonStarter.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 04 2011
*
*/
#include "samson/common/Process.h"            // Process, ProcessVector
#include "samson/common/ports.h"              // CONTROLLER_PORT, WORKER_PORT
#include "samson/network/Network2.h"          // Network2
#include "SamsonStarter.h"                    // Own interface



/* ****************************************************************************
*
* SamsonStarter constructor - 
*/
SamsonStarter::SamsonStarter()
{
	samson::EndpointManager* epMgr = new samson::EndpointManager(samson::Endpoint2::Setup);

	networkP = new samson::Network2(epMgr);
}



/* ****************************************************************************
*
* procVecCreate - 
*/
void SamsonStarter::procVecCreate(const char* controllerHost, int workers, const char* ips[])
{
	int                    size = sizeof(samson::ProcessVector) + workers * sizeof(samson::Process) + sizeof(samson::Process);
	Host*                  hostP;
	samson::ProcessVector* pv;
	samson::Process*       p;
	samson::Endpoint2*     ep;
	int                    spawnerId = 0;

	if ((long) ips[0] != workers)
		LM_X(1, ("%d workers specified on command line, but %d ips in ip-list", workers, (long) ips[0]));

	pv = (samson::ProcessVector*) calloc(1, size);
	if (pv == NULL)
		LM_X(1, ("error allocating %d bytes for process vector", size));
	pv->processes      = workers + 1;
	pv->processVecSize = size;

	LM_M(("Process Vector of %d processes:", pv->processes));
	for (int ix = 0; ix < pv->processes; ix++)
	{
		p = &pv->processV[ix];

		if (ix == 0) // Controller
		{
			snprintf(p->name,        sizeof(p->name),  "Controller");
			snprintf(p->alias,       sizeof(p->alias), "Controller");
			snprintf(p->host,        sizeof(p->host),   "%s", controllerHost);

			p->port = CONTROLLER_PORT;
			p->type = samson::PtController;
		}
		else // Worker
		{
			snprintf(p->name,        sizeof(p->name),  "Worker");
			snprintf(p->alias,       sizeof(p->alias), "Worker%02d", ix - 1);
			snprintf(p->host,        sizeof(p->host),   "%s", ips[ix]);

			p->port = CONTROLLER_PORT;
			p->type = samson::PtWorker;
		}

		hostP = networkP->epMgr->hostMgr->lookup(p->host);
		if (hostP == NULL)
		{
			LM_W(("The host '%s' not found in host manager - adding it", p->host));
			hostP = networkP->epMgr->hostMgr->insert(p->host, NULL);
			if (hostP == NULL)
				LM_X(1, ("error adding host '%s'", p->host));
		}

		snprintf(p->host, sizeof(p->host), "%s", hostP->name);
		snprintf(p->traceLevels, sizeof(p->traceLevels), "0");

		p->state     = samson::Endpoint2::Unused;
		p->verbose   = false;
		p->debug     = false;
		p->reads     = false;
		p->writes    = false;
		p->hidden    = false;
		p->toDo      = false;
		p->id        = (ix == 0)? 0 : ix - 1;

		if (networkP->epMgr->lookup(samson::Endpoint2::Spawner, hostP->name) == NULL)
		{
			if ((ep = networkP->epMgr->add(samson::Endpoint2::Spawner, spawnerId, hostP, SPAWNER_PORT)) == NULL)
				LM_X(1, ("Error creating Spawner Endpoint for host %s", hostP->name));

			++spawnerId;
		}
		else
			LM_W(("Spawner endpoint for %s already created", hostP->name));

		LM_M(("  Process %02d: %s%d@%s", ix, samson::Endpoint2::typeName((samson::Endpoint2::Type) p->type), p->id, p->host));
	}

	networkP->epMgr->procVecSet(pv, false);
	spawners = spawnerId;
}



/* ****************************************************************************
*
* connect - 
*/
samson::Endpoint2::Status SamsonStarter::connect(void)
{
	samson::Endpoint2*        ep;
	samson::Endpoint2::Status s;

	for (int ix = 0; ix < spawners; ix++)
	{
		ep = networkP->epMgr->lookup(samson::Endpoint2::Spawner, ix);
		if (ep == NULL)
			LM_X(1, ("Cannot find Spawner %d", ix));

		LM_T(LmtConnect, ("Connecting to Spawner in %s", ep->hostGet()->name));
		if ((s = ep->connect()) != samson::Endpoint2::OK)
			LM_RE(s, ("Error connecting to %s: %s", ep->name(), ep->status(s)));
	}

	return samson::Endpoint2::OK;
}



/* ****************************************************************************
*
* reset - 
*/
samson::Endpoint2::Status SamsonStarter::reset(void)
{
	int msgs;

    msgs = networkP->epMgr->multiSend(samson::Endpoint2::Spawner, samson::Message::Reset);

	if (msgs != spawners)
		LM_W(("Sent Reset to %d spawners, should be %d ...", msgs, spawners));

	return samson::Endpoint2::OK;
}



/* ****************************************************************************
*
* processList - 
*/
samson::Endpoint2::Status SamsonStarter::processList(void)
{
	return samson::Endpoint2::Error;
}



/* ****************************************************************************
*
* procVecSend - 
*/
samson::Endpoint2::Status SamsonStarter::procVecSend(void)
{
	int                    msgs;
	samson::ProcessVector* procVec = networkP->epMgr->procVecGet();

	msgs = networkP->epMgr->multiSend(samson::Endpoint2::Spawner,
									  samson::Message::ProcessVector,
									  procVec,
									  procVec->processVecSize);

	if (msgs != spawners)
		LM_W(("Sent ProcessVector to %d spawners, should be %d ...", msgs, spawners));

	return samson::Endpoint2::OK;
}



/* ****************************************************************************
*
* run - 
*/
void SamsonStarter::run(void)
{
	networkP->run();
}
