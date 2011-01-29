#ifndef _H_NETWORK_INTERFACE
#define _H_NETWORK_INTERFACE

/* ****************************************************************************
*
* FILE                     NetworkInterface.h - network interface
*
*/
#include <assert.h>				// assert

#include "logMsg.h"             // LM_*
#include "traceLevels.h"        // LmtNetworkInterface, ...

#include "Message.h"            // ss::Message::MessageCode
#include "Endpoint.h"           // ss::Endpoint::Type
#include "Lock.h"				// au::Lock



namespace ss {



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
	virtual      ~DataReceiverInterface() {};
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
		
	// Notify that a nessage has been received.
	// If 'receive' returns ZERO, a JobDone is sent as Ack.
	// If non-zero is returned, a JobError is sent as Ack.
	
	int _receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{
		LM_T(LmtNetworkInterface, ("NETWORK_INTERFACE Received packet type %s",messageCode(msgCode)));
		return receive(fromId, msgCode, packet);
	}

	virtual int receive(int fromId, Message::MessageCode msgCode, Packet* packet) = 0;
	
	
	// Notify that a worker has died 
	virtual void notifyWorkerDied(int worker)
	{
		// This call has to be overwritten by SamsonController
	}

	// Method to recover the status of this element in JSON format
	// In the controllor case, this is exposed to a simple port similar to a telnet service
	virtual std::string getJSONStatus(std::string in)
	{
		LM_M(("JSON request: '%s'", in.c_str()));
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
* NetworkInterface - interface of the interconnection element ( Network and NetworkSimulator )
*/
class  NetworkInterface
{
	au::Lock lock_send;	// Lock to protect multi-thread access to lock
		
public:
	virtual ~NetworkInterface() {};
			
	// Inform about everything ready to start
	virtual bool ready() = 0;
			
	virtual void init(Endpoint::Type type, const char* alias, unsigned short port = 0, const char* controllerName = NULL) {};
	virtual void initAsSamsonController(void) = 0;

	// Set the receiver element (this should be notified about the package)
	virtual void setPacketReceiver( PacketReceiverInterface* receiver) = 0;
	virtual void setDataReceiver(DataReceiverInterface* receiver)       { LM_X(1, ("Please implement setDataReceiverInterface")); };
	virtual void setReadyReceiver(ReadyReceiverInterface* receiver)     { LM_X(1, ("Please implement setReadyReceiverInterface")); };

	// Get identifiers of known elements
	virtual int controllerGetIdentifier()  = 0;		// Get the identifier of the controller
	virtual int workerGetIdentifier(int i) = 0;		// Get the identifier of the i-th worker
	virtual int getMyidentifier()          = 0;		// Get my identifier
	virtual int getNumWorkers()            = 0;		// Get the number of workers
	virtual int getNumEndpoints() { return 0; }     // Get the number of endpoints

	virtual std::string getState(std::string selector) { return std::string("No network state available"); }
		
	// Get the "worker cardinal" from the idenfitier
	// This method should return a value between 0 and (num_workers - 1) or -1 if the identifier provided is not related to any workers
	virtual int getWorkerFromIdentifier(int identifier) = 0;

	// Main run loop control to the network interface
	virtual void run() = 0;
			
	// Suspend the network interface, close everything and return the "run" call
	virtual void quit()=0;					

	// Handy function to get my workerID
	int getWorkerId()
	{
		return getWorkerFromIdentifier(getMyidentifier());
	}
		
	// Run the "run" method in a background process
	void runInBackground();
		
	// Send a packet (return a unique id to inform the notifier later)
	size_t send(PacketSenderInterface* sender, int endpointId, ss::Message::MessageCode code, Packet* packetP );
		
protected:
	virtual size_t _send(PacketSenderInterface* sender, int endpointId, ss::Message::MessageCode code, Packet* packetP ) = 0;
};

}

#endif
