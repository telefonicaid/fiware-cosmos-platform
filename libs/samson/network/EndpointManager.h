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

#include "au/Token.h"                       // au::Token

#include "samson/common/status.h"
#include "samson/common/Process.h"
#include "NetworkInterface.h"
#include "Host.h"
#include "HostMgr.h"
#include "Endpoint2.h"



namespace samson
{



/* ****************************************************************************
*
* Classes
*/
class ListenerEndpoint;
class WebListenerEndpoint;
class NetworkInterface;


/* ****************************************************************************
*
* EpMgrCallback - the type for a callback function
*/
typedef void (*EpMgrCallback)(void* x, void* userParam);



/* ****************************************************************************
*
* EndpointManager - 
*/
class EndpointManager
{
	friend class Network2;
	friend class Endpoint2;
	friend class UnhelloedEndpoint;

    au::Token token;       // Token  to protect the vector of endpoints from multi-thread access
    
public:
	enum CallbackId
	{
		Periodic = 0,
		Timeout,
		CallbackIds
	};

	struct Callback
	{
		EpMgrCallback    func;
		void*            userParam;
	};

private:
	Endpoint2**               endpoint;
	unsigned int              endpoints;
	unsigned int              workers;
	int                       delilahId;
	int                       tmoSecs;
	int                       tmoUSecs;
	ProcessVector*            procVec;
	struct Callback           callback[CallbackIds];

public:
	HostMgr*                  hostMgr;

	Endpoint2*                me;
	Endpoint2*                controller;
	ListenerEndpoint*         listener;
	WebListenerEndpoint*      webListener;
	PacketReceiverInterface*  packetReceiver;
    
    NetworkInterface* networkInterface;

	EndpointManager(Endpoint2::Type _type, const char* controllerIp = NULL);
	~EndpointManager();

	void               initWorker();
	void               initController();
	void               initDelilah(const char* controllerIp);
	void               initSupervisor();
	void               initStarter();

	int                procVecSet(ProcessVector* _procVec, bool save = true);
	ProcessVector*     procVecGet(void);

	void               timeout(void);
	void               periodic(void);
	void               workersAdd(void);
	void               workersConnect(void);
	void               controllerConnect(void);

	Endpoint2*         add(Endpoint2* ep);
	Endpoint2*         add(Endpoint2::Type type, int id, Host* host, unsigned short port, int rFd = -1, int wFd = -1);
	void               remove(Endpoint2* ep);
	Endpoint2*         get(unsigned int index);
	Endpoint2*         get(unsigned int index, int* rFdP);
	Endpoint2*         lookup(Endpoint2::Type type, const char* ip);
	Endpoint2*         lookup(Endpoint2::Type type, Host* host);
	Endpoint2*         lookup(Endpoint2::Type type, int id = 0, int* ixP = NULL);
	int                ixGet(Endpoint2* ep);
	Endpoint2*         indexedGet(unsigned int ix);
	void               show(const char* why, bool forced = false);

	int                endpointCount(Endpoint2::Type type);
	int                endpointCount();
	int                endpointCapacity();
    
	void               tmoSet(int secs, int usecs);  // Set timeout for select loop
	void               run(bool oneShot);            // Main run loop - loops forever, unless 'oneShot' is true ...
	void               callbackSet(CallbackId id, EpMgrCallback func, void* userParam);

	void               setPacketReceiver(PacketReceiverInterface* receiver);

	void               send(int endpointIx, Packet* packetP);
	int                multiSend(Endpoint2::Type type, Packet* packetP);
	int                multiSend(Endpoint2::Type typ, Message::MessageCode code, void* dataP = NULL, int dataLen = 0);
	void               pingTest(void);
};

}

#endif
