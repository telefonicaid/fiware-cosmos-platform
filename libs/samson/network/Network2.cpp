/* ****************************************************************************
*
* FILE                     Network.cpp - Definition for the network interface
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 08 2011
*
*/
#include <sys/select.h>

#include "logMsg/logMsg.h"
#include "logMsg/traceLevels.h"

#include "NetworkInterface.h"
#include "Host.h"
#include "HostMgr.h"
#include "Endpoint2.h"
#include "EndpointManager.h"
#include "Packet.h"
#include "Network2.h"



namespace samson
{



/* ****************************************************************************
*
* Constructor
*/
Network2::Network2(EndpointManager* _epMgr)
{
	epMgr          = _epMgr;
	packetReceiver = NULL;
}



/* ****************************************************************************
*
* Destructor
*/
Network2::~Network2()
{
	if (epMgr)
		delete epMgr;

	google::protobuf::ShutdownProtobufLibrary();
}



/* ****************************************************************************
*
* tmoSet - 
*/
void Network2::tmoSet(int secs, int usecs)
{
	epMgr->tmoSet(secs, usecs);
}



/* ****************************************************************************
*
* run
*/
void Network2::run(bool oneShot)
{
	epMgr->run(oneShot);
}



/* ****************************************************************************
*
* run - 
*/
void Network2::run(void)
{
	epMgr->run(false);
}



/* ****************************************************************************
*
* setPacketReceiver - set the element to be notified when packages arrive
*/
void Network2::setPacketReceiver(PacketReceiverInterface* receiver)
{
	epMgr->setPacketReceiver(receiver);
}



/* ****************************************************************************
*
* ready - 
*/
bool Network2::ready(void)
{
	Endpoint2* controller;

	if ((controller = epMgr->lookup(Endpoint2::Controller)) == NULL)
		return false;

	if (controller->stateGet() == Endpoint2::Ready)
		return true;

	return false;
}



/* ****************************************************************************
*
* initAsSamsonController - 
*/
void Network2::initAsSamsonController(void)
{
}



/* ****************************************************************************
*
* controllerGetIdentifier - 
*/
int Network2::controllerGetIdentifier(void)
{
	int ix;

	epMgr->lookup(Endpoint2::Controller, 0, &ix);

	LM_T(LmtIdIx, ("returning ix %d", ix));
	return ix;
}



/* ****************************************************************************
*
* workerGetIdentifier - 
*/
int Network2::workerGetIdentifier(int id)
{
	int ix;

	epMgr->lookup(Endpoint2::Worker, id, &ix);

	LM_T(LmtIdIx, ("returning ix %d", ix));
	return ix;
}



/* ****************************************************************************
*
* getMyidentifier -
*/
int Network2::getMyidentifier(void)
{
	int ix;

	epMgr->lookup(epMgr->me->typeGet(), epMgr->me->idGet(), &ix);

	LM_T(LmtIdIx, ("returning ix %d", ix));
	return ix;
}



/* ****************************************************************************
*
* getNumWorkers - 
*/
int Network2::getNumWorkers(void)
{
	int noOfWorkers = epMgr->endpointCount(Endpoint2::Worker);

	LM_T(LmtIdIx, ("returning %d workers", noOfWorkers));
	return noOfWorkers;
}



/* ****************************************************************************
*
* getWorkerFromIdentifier - 
*/
int Network2::getWorkerFromIdentifier(int endpointIx)
{
	Endpoint2* ep;
	int        id;

	ep = epMgr->get(endpointIx);

	if (ep == NULL)
		return -1;

	id = ep->idGet();

	LM_T(LmtIdIx, ("returning worker id %d for endpointIx %d", id, endpointIx));
	return id;
}



/* ****************************************************************************
*
* quit - 
*/
void Network2::quit(void)
{
	LM_X(1, ("quit requested, so I quit ..."));
}



/* ****************************************************************************
*
* send - 
*/
size_t Network2::send(PacketSenderInterface* psi, int id, samson::Packet* packetP)
{
	LM_T(LmtSend, ("Sending a packet (%s) to endpoint %d (packet at %p)", messageCode(packetP->msgCode), id, packetP));
	epMgr->show("sending a packet");

	if ((id < 0) || ((unsigned int) id > epMgr->endpoints))
		LM_X(1, ("Bad id %d", id));

	_send(NULL, id, packetP);
	return 0;
}



/* ****************************************************************************
*
* _send - 
*/
void Network2::_send(PacketSenderInterface* psi, int id, samson::Packet* packetP)
{
	epMgr->send(id, packetP);
}



/* ****************************************************************************
*
* delilahSend - 
*/
void Network2::delilahSend(PacketSenderInterface* psi, Packet* packetP)
{
	epMgr->multiSend(Endpoint2::Delilah, packetP);
}



/* ****************************************************************************
*
* endpointListShow - 
*/
void Network2::endpointListShow(const char* why, bool forced)
{
	epMgr->show(why, forced);
}



/* ****************************************************************************
*
* getState - 
*/
std::string Network2::getState(std::string selector)
{
	std::string  output;
	char         partString[256];
	char         conSign;
	Endpoint2*   ep;
	int          eps = 0;

	output = "";

	for (unsigned int ix = 0; ix < epMgr->endpoints; ix++)
	{
		ep = epMgr->endpoint[ix];
		if (ep == NULL)
			continue;

		if ((ep->stateGet() == Endpoint2::Connected || ep->stateGet() == Endpoint2::Ready) && (ep->rFdGet() >= 0))
		{
			++eps;
			conSign = '+';
		}
		else
			conSign = '-';

		snprintf(partString, sizeof(partString), "%c%c%02d: %-35s %-20s %04d fd: %02d  (in: %03d/%s, out: %03d/%s)\n",
				 conSign,
				 (ep->isThreaded() == true)? 's' : '-',
				 ix,
				 ep->name(),
				 ep->stateName(),
				 ep->portGet(),
				 ep->rFdGet(),
				 ep->msgsIn,
				 au::Format::string(ep->bytesIn, "B").c_str(),
				 ep->msgsOut,
				 au::Format::string(ep->bytesOut, "B").c_str());

		output += partString;
	}

	snprintf(partString, sizeof(partString), "Connected to %d endpoints:\n", eps);
	return std::string(partString) + output;
}



/* ****************************************************************************
*
* isConnected - 
*/
bool Network2::isConnected(unsigned int identifier)
{
	if (identifier >= epMgr->endpoints)
		return false;

	if (epMgr->endpoint[identifier] == NULL)
		return false;

	if (epMgr->endpoint[identifier]->state == Endpoint2::Connected)
		return true;
	if (epMgr->endpoint[identifier]->state == Endpoint2::Loopback)
		return true;
	if (epMgr->endpoint[identifier]->state == Endpoint2::Ready)
		return true;

	return false;
}

}
