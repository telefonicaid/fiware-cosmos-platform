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

#include "NetworkInterface.h"	// ss:NetworkInterface
#include "Host.h"               // Host
#include "HostMgr.h"            // HostMgr
#include "Endpoint2.h"          // Endpoint2
#include "EndpointManager.h"    // EndpointManager
#include "Packet.h"             // google::
#include "Network2.h"           // Own interface



namespace ss
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

	epMgr->tmoSet(2, 0);
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
	return ix;
}



/* ****************************************************************************
*
* getNumWorkers - 
*/
int Network2::getNumWorkers(void)
{
	return epMgr->endpointCount(Endpoint2::Worker);
}



/* ****************************************************************************
*
* getWorkerFromIdentifier - 
*/
int Network2::getWorkerFromIdentifier(int endpointIx)
{
	Endpoint2* ep;

	ep = epMgr->get(endpointIx);

	if (ep == NULL)
		return -1;

	return ep->idGet();
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
size_t Network2::send(PacketSenderInterface* psi, int id, ss::Packet* packetP)
{
	size_t r;

	// writeSem.lock();
	r = epMgr->send(psi, id, packetP);
	// writeSem.unlock();

	return r;
}



/* ****************************************************************************
*
* _send - 
*/
size_t Network2::_send(PacketSenderInterface* psi, int id, ss::Packet* packetP)
{
	return epMgr->send(psi, id, packetP);
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
