#ifndef ENDPOINT_MGR_H
#define ENDPOINT_MGR_H

/* ****************************************************************************
*
* FILE                      EndpointMgr.h
*
* DESCRIPTION               Class for endpoint manager
*
*/
#include "Message.h"           // MessageType
#include "Endpoint.h"          // Endpoint
#include "NetworkInterface.h"  // PacketReceiverInterface


namespace ss {



class EndpointMgr
{
private:
    PacketReceiverInterface* receiver;
	NetworkInterface*        network;

public:
	EndpointMgr(NetworkInterface* network, int endpoints, int workers);
	void packetReceiverSet(PacketReceiverInterface* receiver) { this->receiver = receiver; }
	
	void        init(Endpoint::Type myType, unsigned short port = 0, const char* controllerName = NULL);

	Endpoint*   endpointAdd(int fd, char* name, int workers, Endpoint::Type type, std::string ip, unsigned short port, int coreNo = -1);
	void        endpointRemove(Endpoint* ep);
	Endpoint*   endpointLookup(int ix);
	Endpoint*   endpointCoreWorkerLookup(int coreNo);
	Endpoint*   endpointLookup(int fd, int* idP);

	void        msgTreat(int fd, char* name);
	void        run(void);

	int         helloSend(Endpoint* ep, Message::MessageType type);
	void        readyCheck(void);

	// To be moved to Main class  (Worker, Controller, CoreWorker, Delilah)
	void workerStatusToController(void);

	// To be removed completely
	std::vector<Endpoint> endpointV;

private:
	Endpoint*   listener;
	Endpoint*   me;
	Endpoint*   controller;

public:
	int         Workers;
	int         Endpoints;
	Endpoint**  endpoint;
	bool        iAmReady;
};

}

#endif
