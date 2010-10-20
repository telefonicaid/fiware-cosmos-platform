/* ****************************************************************************
*
* FILE                     Network.h - Definition for the network interface
*
*/

#pragma once

#include <cstring>				// size_t
#include <vector>				// vector

#include "Endpoint.h"			// Endpoint
#include "NetworkInterface.h"	// ss:NetworkInterface 


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

	Endpoint*                me;
	Endpoint*                listener;
	Endpoint*                controller;
	Endpoint*                delilah;
	Endpoint*                temporal;
	std::vector<Endpoint>    endpointV;

	void init(Endpoint::Type, unsigned short port = 0);
	void ipSet(char* ip);

public:
	Network();
		
	void setPacketReceiver(PacketReceiverInterface* receiver);

	virtual void initAsSamsonController(int port, std::vector<std::string> peers);
	virtual void initAsSamsonWorker(int localPort, std::string controllerEndpoint);
	virtual void initAsDelilah(std::string controllerEndpoint);
	
	virtual void setPacketReceiverInterface( PacketReceiverInterface* receiver);
	
	
	bool ready();                                   // Inform about everything ready
                                                    // The controller expects all the workers to be connected
                                                    // The worker expects to be connected with all the workers and the controller
                                                    // Delilah expects to be connected with all the workers and the contorller

	// Get identifiers of known elements
	virtual int controllerGetIdentifier(){return 0;};		// Get the identifier of the controller
	virtual int workerGetIdentifier(int i){return 0;};		// Get the identifier of the i-th worker
	virtual int getMyidentifier(){return 0;};				// Get my identifier
	virtual int getNumWorkers(){return 0;};					// Get the number of workers
	
	// Send a packet (return a unique id to inform the notifier later)
	virtual size_t send(Packet* packet, int toIdentifier, PacketSenderInterface* sender){return 0;};
	
	Endpoint* listenerGet();                        // Get listener endPoint
	Endpoint* meGet();                              // Get my endPoint
	Endpoint* controllerGet();                      // Get the endPoint of the controller
	Endpoint* workerGet(int i);                     // Get the endPoint of the "i-th" worker

	int worker( Endpoint* endPoint );                // Identify the worker of this endPoint ( 0 if the controller )

	std::vector<Endpoint*> endPoints();              // Get a list of all endPoints
	std::vector<Endpoint*> samsonWorkerEndpoints();  // Get a list of the samsonWorkers endpoints

	// Send a packet (return a unique id to inform the notifier later)
	size_t send(Packet* packet, Endpoint* endpoint, PacketSenderInterface* sender);

	/**
	   This function is expected to return only if quit() function is called
	*/
	void run();                                     // Main run loop control to the network interface

	// Syspend the network interface, close everything and return the "run" call
	void quit();

private:
	bool iAmReady;

	void       endpointAdd(int fd, char* name, int workers, Endpoint::Type type, std::string ip, unsigned short port);
	Endpoint*  endpointLookupByFd(int fd);
	Endpoint*  endpointLookupByIpAndPort(const char* ip, unsigned short port);

	void msgTreat(int fd, char* name);
	void checkInitDone(void);
	int  helloSend(int fd, char* name);
};

	
}
