#ifndef ENDPOINT_MANAGER_H
#define ENDPOINT_MANAGER_H

/* ****************************************************************************
*
* FILE                     EndpointManager.h - 
*
* AUTHOR                   Ken Zangelin
*
* CREATION DATE            Apr 06 2011
*
*/
#include "NetworkInterface.h"	// NetworkInterface 
#include "Host.h"               // Host
#include "HostMgr.h"            // HostMgr
#include "Process.h"            // ProcessVector
#include "Endpoint2.h"          // Endpoint2



namespace ss
{


/* ****************************************************************************
*
* Classes
*/
class ListenerEndpoint;
class WebListenerEndpoint;



/* ****************************************************************************
*
* EndpointManager - 
*/
class EndpointManager
{
private:
	Endpoint2**               endpoint;
	unsigned int              endpoints;
	unsigned int              workers;

	int                       tmoSecs;
	int                       tmoUSecs;
	ProcessVector*            procVec;

public:
	HostMgr*                  hostMgr;
	
	Endpoint2*                me;
	Endpoint2*                controller;
	ListenerEndpoint*         listener;
	WebListenerEndpoint*      webListener;
	PacketReceiverInterface*  packetReceiver;
	DataReceiverInterface*    dataReceiver;

	EndpointManager(Endpoint2::Type _type, const char* controllerIp = NULL);
	~EndpointManager();

	void               initWorker();
	void               initController();
	void               initSpawner();
	void               initDelilah(const char* controllerIp);
	void               initSupervisor();

	int                procVecSet(ProcessVector* _procVec);
	ProcessVector*     procVecGet(void);
	Endpoint2::Status  setupAwait(void);

	Endpoint2*         add(Endpoint2* ep);
	Endpoint2*         add(Endpoint2::Type type, int id, const char* name, const char* alias, Host* host, unsigned short port, int rFd = -1, int wFd = -1);

	void               remove(Endpoint2* ep);
	Endpoint2*         get(unsigned int index);
	Endpoint2*         get(unsigned int index, int* rFdP);
	Endpoint2*         lookup(Endpoint2::Type type, const char* ip);
	Endpoint2*         lookup(Endpoint2::Type type, int id, int* ixP);
	Endpoint2*         lookup(const char* alias);
	void               list(const char* why, bool forced = false);
	int                endpointCount(Endpoint2::Type type);
	void               show(const char* why, bool forced = false);
    
	void               tmoSet(int secs, int usecs);  // Set timeout for select loop
	void               run(bool oneShot);            // Main run loop - loops forever, unless 'oneShot' is true ...

	void               setPacketReceiver(PacketReceiverInterface* receiver);
	void               setDataReceiver(DataReceiverInterface* receiver);

	size_t             send(PacketSenderInterface* psi,      int endpointIx,       Packet* packetP);
	int                multiSend(PacketSenderInterface* psi, Endpoint2::Type type, Packet* packetP);
	int                multiSend(Endpoint2::Type typ, Message::MessageCode code, void* dataP = NULL, int dataLen = 0);
};

}

#endif
