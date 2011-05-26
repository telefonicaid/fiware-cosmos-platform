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
	dataReceiver   = NULL;
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
* setPacketReceiver - 
*/
void Network2::setPacketReceiver(PacketReceiverInterface* receiver)
{
	epMgr->setPacketReceiver(receiver);
}




/* ****************************************************************************
*
* setDataReceiver - set the element to be notified when packages arrive
*/
void Network2::setDataReceiver(DataReceiverInterface* receiver)
{
	epMgr->setDataReceiver(receiver);
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
*
* NOTE
*  epMgr->me is not in endpoint vector so this will not work.
*  Need to know why this info is necessary ...
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
	LM_T(LmtSend, ("Sending a packet (%s) to endpoint %d (psi at %p, packet at %p)", messageCode(packetP->msgCode), id, psi, packetP));
	epMgr->show("sending a packet", true);
	_send(psi, id, packetP);
	return 0;
}



/* ****************************************************************************
*
* _send - 
*/
void Network2::_send(PacketSenderInterface* psi, int id, samson::Packet* packetP)
{
	epMgr->send(psi, id, packetP);
}



/* ****************************************************************************
*
* delilahSend - 
*/
void Network2::delilahSend(PacketSenderInterface* psi, Packet* packetP)
{
	epMgr->multiSend(psi, Endpoint2::Delilah, packetP);
}



/* ****************************************************************************
*
* endpointListShow - 
*/
void Network2::endpointListShow(const char* why, bool forced)
{
	epMgr->show(why, forced);
}

}
