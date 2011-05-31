/* ****************************************************************************
*
* FILE                     ControllerEndpoint.h
*
* DESCRIPTION              Class for controller endpoints
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            May 02 2011
*
*/
#include <unistd.h>               // close
#include <fcntl.h>                // F_SETFD
#include <pthread.h>              // pthread_t

#include "logMsg/logMsg.h"               // LM_*
#include "logMsg/traceLevels.h"          // Lmt*

#include "samson/common/ports.h"                // CONTROLLER_PORT
#include "samson/network/EndpointManager.h"      // EndpointManager
#include "ControllerEndpoint.h"   // Own interface



namespace samson
{



/* ****************************************************************************
*
* ControllerEndpoint - 
*/
ControllerEndpoint::ControllerEndpoint
(
	EndpointManager*  _epMgr,
	const char*       _name,
	const char*       _alias,
	Host*             _host,
	int               _rFd,
	int               _wFd
) : Endpoint2(_epMgr, Controller, 0, _name, _alias, _host, CONTROLLER_PORT, _rFd, _wFd)
{
}



/* ****************************************************************************
*
* ~ControllerEndpoint - 
*/
ControllerEndpoint::~ControllerEndpoint() // : ~Endpoint2()
{
}



/* ****************************************************************************
*
* msgTreat2 - 
*/
Endpoint2::Status ControllerEndpoint::msgTreat2(Message::Header* headerP, void* dataP, int dataLen, Packet* packetP)
{
	ProcessVector* pVec;

	LM_T(LmtMsgTreat, ("Treating %s %s from %s%d@%s", messageCode(headerP->code), messageType(headerP->type), typeName(), id, host->name));
	switch (headerP->code)
	{
	case Message::ProcessVector:
		pVec = (ProcessVector*) dataP;
		LM_T(LmtProcessVector, ("Setting ProcessVector (%d processes, size: %d)", pVec->processes, pVec->processVecSize));
		epMgr->procVecSet(pVec, false);
		epMgr->workersAdd();
		epMgr->workersConnect();
		epMgr->show("Delilah connected to Workers");
		break;

	default:
		if (epMgr->packetReceiver == NULL)
			LM_X(1, ("No packetReceiver (SW bug) - got a '%s' %s from %s%d@%s", messageCode(headerP->code), messageType(headerP->type), typeName(), id, host->name));

		epMgr->packetReceiver->_receive(packetP);
		break;
	}

	return OK;
}



/* ****************************************************************************
*
* msgTreat2 - 
*/
Endpoint2::Status ControllerEndpoint::msgTreat2(void)
{
	LM_X(1, ("Nothing done here - frankly, I thought ListenerEndpoint::msgTreat2 would be called ..."));
	return Error;
}

}
