/* ****************************************************************************
*
* FILE                     network.h - Definition for the network interface
*
*/

#pragma once

#include <cstring>		  // size_t
#include <vector>		  // vector

#include "Endpoint.h"     // Endpoint



namespace ss {

	
class Endpoint;
class Packet;
	


/* ****************************************************************************
*
* PacketReceiverInterface - 
*/
class PacketReceiverInterface
{
public:
	// Notification that a particular send finished
	virtual void receive(Packet* packet, Endpoint* from) = 0;
};



/* ****************************************************************************
*
* PacketSenderInterface - packet sender that needs receive notification
*/
class PacketSenderInterface
{
public:
	// Notification that the packet has been sent
	virtual void notificationSent(size_t id, bool success) = 0;
};
	


/* ****************************************************************************
*
* NetworkInterface - main element that interconnects
*                      o samsonController
*                      o samsonWorker
*                      o delilah
*/
class NetworkInterface
{
	PacketReceiverInterface* receiver;

	Endpoint*                me;
	Endpoint*                controller;
	Endpoint*                delilah;
	std::vector<Endpoint>    endpointV;

	void init(Endpoint* me);
	void ipSet(char* ip);
public:
	NetworkInterface();
		
	void setPacketReceiver(PacketReceiverInterface* receiver);

	void initAsSamsonController(Endpoint myEndpoint, std::vector<Endpoint> peers);
	void initAsSamsonWorker(Endpoint myEndpoint, Endpoint controllerEndpoint);
	void initAsDelilah(Endpoint controllerEndpoint);

	bool ready();                                   // Inform about everything ready
                                                    // The controller expects all the workers to be connected
                                                    // The worker expects to be connected with all the workers and the controller
                                                    // Delilah expects to be connected with all the workers and the contorller

	Endpoint* meGet();                              // Get my endPoint
	Endpoint* controllerGet();                      // Get the endPoint of the controller
	Endpoint* workerGet(int i);                     // Get the endPoint of the "i-th" worker

	int worker( Endpoint endPoint );                // Identify the worker of this endPoint ( 0 if the controller )

	std::vector<Endpoint> endPoints();              // Get a list of all endPoints
	std::vector<Endpoint> samsonWorkerEndpoints();  // Get a list of the samsonWorkers endpoints

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

	void endpointAdd(int fd, char* hostName);
	void msgTreat(Endpoint* epP);
	void checkInitDone(void);

	void helloSend(Endpoint* epP);
};

	
}
