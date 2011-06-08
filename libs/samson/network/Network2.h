#ifndef NETWORK2_H
#define NETWORK2_H

/* ****************************************************************************
*
* FILE                     Network.h - Definition for the network interface
*
*/
#include <cstring>				// size_t
#include <vector>				// vector

#include "samson/network/Endpoint2.h"          // Endpoint2
#include "samson/network/EndpointManager.h"    // EndpointManager
#include "samson/network/NetworkInterface.h"   // ss:NetworkInterface



namespace samson
{

	

/* ****************************************************************************
*
* Network2 - main element that interconnects
*                       o samsonController
*                       o samsonWorker
*                       o delilah
*/
class Network2 : public NetworkInterface
{
public:
	EndpointManager*  epMgr;
	
	Network2(EndpointManager* epMgr);
	~Network2();

	void  tmoSet(int secs, int usecs);  // Set timeout for select loop
	void  run(bool oneShot);            // Main run loop - loops forever, unless 'oneShot' is true ...
	void  endpointListShow(const char* why, bool forced = false);


	//
	// Callbacks
	//
	void  setPacketReceiver(PacketReceiverInterface* receiver);

	PacketReceiverInterface*   packetReceiver;



	//
	// NetworkInterface stuff
	//
	bool         ready(void);
	void         initAsSamsonController(void);
	int          controllerGetIdentifier(void);
	int          workerGetIdentifier(int);
	int          getMyidentifier(void);
	int          getNumWorkers(void);
	int          getWorkerFromIdentifier(int);
	void         run(void);
	void         quit(void);
	void         delilahSend(PacketSenderInterface*, Packet*);
	void         _send(PacketSenderInterface*, int, Packet*);
	size_t       send(PacketSenderInterface*, int, Packet*);
	std::string  getState(std::string selector = NULL);
	bool         isConnected(unsigned int identifier);
};

}

#endif
