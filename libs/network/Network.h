#ifndef _H_NETWORK
#define _H_NETWORK

/* ****************************************************************************
*
* FILE                     Network.h - Definition for the network interface
*
*/
#include <cstring>				// size_t
#include <vector>				// vector

#include "Endpoint.h"			// Endpoint
#include "Message.h"            // ss::Message::MessageCode
#include "NetworkInterface.h"	// ss:NetworkInterface 



#define WORKERS       5
#define DELILAHS     20
#define CORE_WORKERS 32
#define TEMPORALS    20



namespace ss {

	
class Endpoint;
class Packet;
	


/* ****************************************************************************
*
* Network - main element that interconnects
*                      o samsonController
*                      o samsonWorker
*                      o delilah
*/
class Network : public NetworkInterface
{
	PacketReceiverInterface* receiver;

	void init(Endpoint::Type type, const char* alias, unsigned short port = 0, const char* controllerName = NULL);
	void ipSet(char* ip);

public:
	Network();
	Network(int endpoints, int workers);

	virtual void setPacketReceiverInterface(PacketReceiverInterface* receiver);

	virtual void initAsSamsonController(int port, int num_workers);
	
	bool ready();                                    // Inform about everything ready
                                                     // The controller expects all the workers to be connected
                                                     // The worker expects to be connected with all the workers and the controller
                                                     // Delilah expects to be connected with all the workers and the contorller

	// Get identifiers of known elements
	virtual int controllerGetIdentifier();	         // Get the identifier of the controller
	virtual int workerGetIdentifier(int nthWorker);  // Get the identifier of the i-th worker
	virtual int getMyidentifier() { return 0; };     // Get my identifier
	virtual int getNumWorkers();                     // Get the number of workers
	virtual int getNumEndpoints(void)                { return sizeof(endpoint) / sizeof(endpoint[0]); }
	virtual int getWorkerFromIdentifier(int identifier);

	// Send a packet (return a unique id to inform the notifier later)
	virtual size_t send(PacketSenderInterface* sender, int endpointId, ss::Message::MessageCode code, Packet* packetP = NULL);
	
	std::vector<Endpoint*> samsonWorkerEndpoints();  // Get a list of the samsonWorkers endpoints

	/**
	   This function is expected to return only if quit() function is called
	*/
	void run();                                     // Main run loop control to the network interface

	// Syspend the network interface, close everything and return the "run" call
	void quit();

public:
	int        Workers;
	int        Endpoints;
	Endpoint** endpoint;

private:
	Endpoint*  listener;
	Endpoint*  me;
	Endpoint*  controller;

	bool       iAmReady;

	Endpoint*  endpointAdd(int rFd, int wFd, const char* name, const char* alias, int workers, Endpoint::Type type, std::string ip, unsigned short port, int core);
	void       endpointRemove(Endpoint* ep);
	Endpoint*  endpointLookup(int fd, int* idP);
	Endpoint*  endpointLookup(int ix);
	Endpoint*  endpointLookup(char* alias);
	Endpoint*  endpointFreeGet(Endpoint::Type type);

	void msgPreTreat(Endpoint* ep, int endpointId);
	void msgTreat(void* vP);
	int  helloSend(Endpoint* ep, Message::MessageType type);

};

	
}

#endif
