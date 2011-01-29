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



namespace ss {

	
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
   void ipSet(char* ip);

public:
	Network(Endpoint::Type type, const char* alias, unsigned short port = 0, int endpoints = 80, int workers = 10);

	void         init(const char* controllerName = NULL);
	Endpoint*    controllerConnect(const char* controllerName);

	virtual void setPacketReceiver(PacketReceiverInterface* receiver);
	virtual void setDataReceiver(DataReceiverInterface* receiver);
	virtual void setEndpointUpdateReceiver(EndpointUpdateReceiverInterface* receiver);
	virtual void setReadyReceiver(ReadyReceiverInterface* receiver);

	PacketReceiverInterface*           packetReceiver;
	DataReceiverInterface*             dataReceiver;
	EndpointUpdateReceiverInterface*   endpointUpdateReceiver;
	ReadyReceiverInterface*            readyReceiver;

	virtual void   initAsSamsonController(void);
	void           fdSet(int fd, const char* name, const char* alias);

	bool           ready();                  // Inform about everything ready
                                             // The controller expects all the workers to be connected
                                             // The worker expects to be connected to all the workers and the controller
                                             // Delilah expects to be connected to all the workers and the controller

	// Get identifiers of known elements
	virtual int controllerGetIdentifier();	         // Get the identifier of the controller
	virtual int workerGetIdentifier(int nthWorker);  // Get the identifier of the i-th worker
	virtual int getMyidentifier() { return 0; };     // Get my identifier
	virtual int getNumWorkers();                     // Get the number of workers
	virtual int getNumEndpoints(void)                { return sizeof(endpoint) / sizeof(endpoint[0]); }
	virtual int getWorkerFromIdentifier(int identifier);

	
	std::vector<Endpoint*> samsonWorkerEndpoints();               // Return the list of 'Worker' endpoints
	std::vector<Endpoint*> samsonEndpoints();                     // Return the list of all endpoints
	std::vector<Endpoint*> samsonEndpoints(Endpoint::Type type);  // Return the list of all 'type' endpoints
	Endpoint*              logServerLookup(void);
	void                   logServerSet(const char* logServerHost);

	/**
	   This function is expected to return only if quit() function is called
	*/
	void run();                                     // Main run loop control to the network interface
	void runUntilReady(void);                       // run the run loop until READY
	int  poll();                                    // Poll endpoint connections ant treat if necessary

	// Syspend the network interface, close everything and return the "run" call
	void quit();

protected:
	// Send a packet (return a unique id to inform the notifier later)
	size_t _send(PacketSenderInterface* sender, int endpointId, ss::Message::MessageCode code, Packet* packetP = NULL);


public:
	int          tmoSecs;
	int          tmoUsecs;
	int          Workers;
	int          Endpoints;
	Endpoint**   endpoint;

	void         msgPreTreat(Endpoint* ep, int endpointId);
	void         msgTreat(void* vP);
	void         controllerMsgTreat(Endpoint* ep, Message::MessageCode msgCode, Message::MessageType msgType, void* dataP, int dataLen, Packet* packetP);
	std::string  getState(std::string selector);

	Endpoint*    endpointAdd(const char* why, int rFd, int wFd, const char* name, const char* alias, int workers, Endpoint::Type type, std::string ip, unsigned short port, int core = -1, Endpoint* inheritFrom = NULL);
	Endpoint*    endpointLookup(int fd, int* idP);
	Endpoint*    endpointLookup(int ix);
	Endpoint*    endpointLookup(char* alias);
	Endpoint*    endpointLookup(Endpoint::Type type, char* ip);
	void         endpointRemove(Endpoint* ep, const char* why);
	void         endpointListShow(const char* why);

	int          helloSend(Endpoint* ep, Message::MessageType type);

	Endpoint*    controller;
	Endpoint*    me;

private:
	Endpoint*       listener;
	Endpoint*       ME;

	bool            iAmReady;
	unsigned short  port;

	void            reset(Endpoint::Type type, const char* alias, unsigned short port = 0, int endpoints = 80, int workers = 10);
	Endpoint*       endpointFreeGet(Endpoint::Type type);
	void            checkAllWorkersConnected(void);

	void            webServiceAccept(Endpoint* ep);
	void            webServiceTreat(Endpoint* ep);
};

	
}

#endif
