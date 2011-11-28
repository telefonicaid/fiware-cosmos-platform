/* ****************************************************************************
*
* FILE                     SamsonStarter.cpp
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 04 2011
*
*/
#include "samson/common/status.h"
#include "samson/common/Process.h"            // Process, ProcessVector
#include "samson/common/ports.h"              // CONTROLLER_PORT, WORKER_PORT
#include "samson/network/Network2.h"          // Network2
#include "SamsonStarter.h"                    // Own interface



namespace samson
{



/* ****************************************************************************
*
* SamsonStarter constructor - 
*/
SamsonStarter::SamsonStarter()
{
	networkP = new Network2(Endpoint2::Starter);
}

SamsonStarter::~SamsonStarter()
{
}


void SamsonStarter::receive(Packet* packetP)
{
}

/* ****************************************************************************
*
* procVecCreate - 
*/
void SamsonStarter::procVecCreate(const char* controllerHost, int workers, const char* ips[])
{
	int            size = sizeof(ProcessVector) + workers * sizeof(Process) + sizeof(Process);
	Host*          hostP;
	ProcessVector* pv;
	Process*       p;
	Endpoint2*     ep;
	int            spawnerId = 0;
	int            wIx       = 1;

	LM_T(LmtHost, ("controllerHost: '%s'", controllerHost));

	if ((long) ips[0] != workers)
		LM_X(1, ("%d workers specified on command line, but %d ips in ip-list", workers, (long) ips[0]));

	pv = (ProcessVector*) calloc(1, size);
	if (pv == NULL)
		LM_X(1, ("error allocating %d bytes for process vector", size));

	pv->processes      = workers + (controllerHost[0] != 0);
	pv->processVecSize = size;
	LM_M(("workers: %d", workers));

	LM_T(LmtProcessVector, ("Process Vector of %d processes:", pv->processes));
	for (int ix = 0; ix < pv->processes; ix++)
	{
		p = &pv->processV[ix];

		if ((ix == 0) && (controllerHost[0] != 0)) // Controller
		{
			snprintf(p->name,        sizeof(p->name),  "Controller");
			snprintf(p->alias,       sizeof(p->alias), "Controller");
			snprintf(p->host,        sizeof(p->host),   "%s", controllerHost);
			LM_T(LmtHost, ("copied host name of size %d: '%s'", sizeof(p->host), p->host));

			p->port = CONTROLLER_PORT;
			p->type = PtController;
		}
		else // Worker
		{
			LM_M(("Adding worker %d", wIx));
			LM_M(("ips[%d]: '%s'", wIx, ips[wIx]));
			snprintf(p->name,        sizeof(p->name),  "Worker");
			snprintf(p->alias,       sizeof(p->alias), "Worker%02d", wIx);
			snprintf(p->host,        sizeof(p->host),   "%s", ips[wIx]);

			p->port = CONTROLLER_PORT;
			p->type = PtWorker;
			++wIx;
		}

		hostP = networkP->epMgr->hostMgr->lookup(p->host);
		if (hostP == NULL)
		{
			LM_T(LmtHost, ("The host '%s' not found in host manager - adding it", p->host));
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
		p->id        = (ix == 0)? 0 : ix - 1;

		if (networkP->epMgr->lookup(Endpoint2::Spawner, hostP->name) == NULL)
		{
			if ((ep = networkP->epMgr->add(Endpoint2::Spawner, spawnerId, hostP, SPAWNER_PORT)) == NULL)
				LM_X(1, ("Error creating Spawner Endpoint for host %s", hostP->name));

			++spawnerId;
		}
		else
			LM_T(LmtProcessVector, ("Spawner endpoint for %s already created", hostP->name));

		LM_T(LmtProcessVector, ("  Process %02d: %s%d@%s", ix, Endpoint2::typeName((Endpoint2::Type) p->type), p->id, p->host));
	}

	networkP->epMgr->procVecSet(pv, false);
	spawners = spawnerId;
}



/* ****************************************************************************
*
* connect - 
*/
Status SamsonStarter::connect(void)
{
	Endpoint2*        ep;
	Status s;

	for (int ix = 0; ix < spawners; ix++)
	{
		ep = networkP->epMgr->lookup(Endpoint2::Spawner, ix);
		if (ep == NULL)
			LM_X(1, ("Cannot find Spawner %d", ix));

		LM_T(LmtConnect, ("Connecting to Spawner in %s", ep->hostGet()->name));
		if ((s = ep->connect()) != OK)
			LM_RE(s, ("Error connecting to %s: %s", ep->name(), status(s)));
	}

	return OK;
}



/* ****************************************************************************
*
* reset - 
*/
Status SamsonStarter::reset(void)
{
	int msgs;

    msgs = networkP->epMgr->multiSend(Endpoint2::Spawner, Message::Reset);

	if (msgs != spawners)
		LM_W(("Sent Reset to %d spawners, should be %d ...", msgs, spawners));

	return OK;
}



/* ****************************************************************************
*
* processList - 
*/
Status SamsonStarter::processList(void)
{
	return Error;
}



/* ****************************************************************************
*
* procVecSend - 
*/
Status SamsonStarter::procVecSend(void)
{
	int             msgs;
	ProcessVector*  procVec = networkP->epMgr->procVecGet();

	msgs = networkP->epMgr->multiSend(Endpoint2::Spawner,
									  Message::ProcessVector,
									  procVec,
									  procVec->processVecSize);

	if (msgs != spawners)
		LM_W(("Sent ProcessVector to %d spawners, should be %d ...", msgs, spawners));

	return OK;
}



/* ****************************************************************************
*
* run - 
*/
void SamsonStarter::run(void)
{
	networkP->run();
}

}
