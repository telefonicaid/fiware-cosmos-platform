#ifndef _H_NETWORK_INTERFACE
#define _H_NETWORK_INTERFACE

/* ****************************************************************************
*
* FILE                     NetworkInterface.h - network interface
*
* AUTHOR                   Andreu Urruela
*
* CREATION DATE            2010
*
*/
#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // LmtNetworkInterface, ...

#include "au/Lock.h"                   // au::Lock

//#include "engine/DiskStatistics.h"      // engine::DiskStatistics

#include "samson/network/Message.h"    // samson::Message::MessageCode
#include "samson/network/Endpoint.h"   // samson::Endpoint::Type

namespace engine
{
    class DiskStatistics;
}

namespace samson {


/* ****************************************************************************
*
* Forward declarations
*/
class Packet;
	


/* ****************************************************************************
*
* DataReceiverInterface - 
*/
class DataReceiverInterface
{
public:
	virtual int   receive(int fromId, int nb, Message::Header* headerP, void* dataP) = 0;
	virtual void  init(ProcessVector* procVec) = 0;
	virtual      ~DataReceiverInterface() {};
};



/* ****************************************************************************
*
* TimeoutReceiverInterface - 
*/
class TimeoutReceiverInterface
{
public:
	virtual int   timeoutFunction() = 0;
	virtual      ~TimeoutReceiverInterface() {};
};



/* ****************************************************************************
*
* EndpointUpdateReceiverInterface - 
*/
class EndpointUpdateReceiverInterface
{
public:
	virtual int endpointUpdate(Endpoint* ep, Endpoint::UpdateReason, const char* reason, void* info = NULL) = 0;
	virtual     ~EndpointUpdateReceiverInterface() {};
};



/* ****************************************************************************
*
* ReadyReceiverInterface - 
*/
class ReadyReceiverInterface
{
public:
	virtual int  ready(const char* info) = 0;
	virtual     ~ReadyReceiverInterface() {};
};



/* ****************************************************************************
*
* PacketReceiverInterface - 
*/
class PacketReceiverInterface
{
public:

	std::string packetReceiverDescription;
	
	// Method to receive a packet
	// It is responsability of this callback to delete the received packet with "delete packet"
	virtual void receive(Packet* packet) = 0;

	// Convenient way to run the receive methods using Engine
	void _receive(Packet* packet);
	
	// Notify that a worker has died 
	virtual void notifyWorkerDied(int worker)
	{
		// This call has to be overwritten by SamsonController
	}

	// Method to recover the status of this element in JSON format
	// In the controllor case, this is exposed to a simple port similar to a telnet service
	virtual std::string getJSONStatus(std::string in)
	{
		return "Not implemented\n";
	}

	virtual ~PacketReceiverInterface() {};
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
	
	virtual ~PacketSenderInterface() {};
};
	

    
/* ****************************************************************************
*
* NetworkInterface - interface of the interconnection element (Network and NetworkSimulator)
*/
class  NetworkInterface
{
	const char* node_name;              // Name of this node for debugging (set with setNodeName)
    
public:

    engine::DiskStatistics *statistics;          // Statistics
    
    
    void setNodeName(const char* _node_name)
    {
        node_name = _node_name;
    }
    
    NetworkInterface();
	virtual ~NetworkInterface();

	// Inform about everything ready to start
	virtual bool ready() = 0;
	virtual bool ready(bool connectedToAllWorkers) { return false; };

	// Init functions of the network element - not used in Network2
	virtual void init(Endpoint::Type type, const char* alias, unsigned short port = 0, const char* controllerName = NULL) {};
	virtual void initAsSamsonController(void) = 0;

	// Set the receiver element (this should be notified about the package)
	virtual void setPacketReceiver(PacketReceiverInterface* receiver) = 0;

	// These two aren't used in Network2
	virtual void setDataReceiver(DataReceiverInterface* receiver)       { LM_X(1, ("Please implement setDataReceiverInterface")); };
	virtual void setReadyReceiver(ReadyReceiverInterface* receiver)     { LM_X(1, ("Please implement setReadyReceiverInterface")); };


	// Get identifiers of known elements
	virtual int controllerGetIdentifier()  = 0;    // Get the identifier of the controller
	virtual int workerGetIdentifier(int i) = 0;    // Get the identifier of the i-th worker
	virtual int getMyidentifier()          = 0;    // Get my identifier
	virtual int getNumWorkers()            = 0;    // Get the number of workers
	virtual int getNumEndpoints() { return 0; }    // Get the number of endpoints

	virtual void jobInfo(int endpointId, int* messages, long long* dataLen)
	{
		*messages = 0;
		*dataLen  = 0;
	}

	// Get information about network state
    virtual void getInfo( std::ostringstream& output )=0;

	// Get the "worker cardinal" from the idenfitier
	// This method should return a value between 0 and (num_workers - 1) or -1 if the identifier provided is not related to any worker
	virtual int getWorkerFromIdentifier(int identifier) = 0;

	// Main run loop control to the network interface
	// this call is a blocking call. It only returns if "quit" is called from another thread
	virtual void run() = 0;

	// Suspend the network interface, close everything and return the "run" call
	virtual void quit(void) = 0;

	// Handy function to get my workerID
	int getWorkerId(void)
	{
		return getWorkerFromIdentifier(getMyidentifier());
	}

	// Run the "run" method in a background thread and return control to the current thread
	void runInBackground();

	virtual bool isConnected(unsigned int identifier) { return true; };
	virtual void delilahSend(PacketSenderInterface* packetSender, Packet* packetP) = 0;

	// New API to send packets    
	void send(int endpoint, Packet* p);
	void sendToWorker(int workerId, Packet* p);
	void sendToController(Packet* p);

private:

	// Send a packet (return a unique id to inform the notifier later)
	virtual size_t send(PacketSenderInterface* sender, int endpointId, Packet* packetP) = 0;
};

}

#endif
