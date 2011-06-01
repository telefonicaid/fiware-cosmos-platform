#ifndef _H_NETWORK_INTERFACE
#define _H_NETWORK_INTERFACE

/* ****************************************************************************
*
* FILE                     NetworkInterface.h - network interface
*
*/

#include "logMsg/logMsg.h"             // LM_*
#include "logMsg/traceLevels.h"        // LmtNetworkInterface, ...

#include "au/Lock.h"				// au::Lock
#include "Message.h"                // samson::Message::MessageCode
#include "ComponentId.h"
#include "ComponentInfo.h"
#include "status.h"


namespace samson {



/* ****************************************************************************
*
* Forward declarations
*/
class Packet;



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
	virtual void receive( Packet* packet ) = 0;

	// Convenient way to run the receive methods using Engine
	void _receive( Packet* packet );
	
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
* NetworkInterface - interface of the interconnection element ( Network and NetworkSimulator )
*/
class  NetworkInterface
{
    
public:
    
	virtual ~NetworkInterface() {};
			
	// Inform about everything ready to start
	virtual bool ready() = 0;
			
	// Init functions of the network element
	virtual void init(ComponentId id) = 0;

	// Set the receiver element (this should be notified about the package)
	virtual void setPacketReceiver( PacketReceiverInterface* receiver) = 0;


	// Get identifiers of known elements
	virtual int           getNumWorkers()        = 0;		// Get the number of workers
	virtual ComponentId   componentId()          = 0;		// Get my identifier
	virtual ComponentInfo componentInfo(ComponentId id) = 0;

	// Get information about network state
	virtual std::string getState(void)     { return std::string("No network state available"); }
		
	// Main run loop control to the network interface
	// this call is a blocking call. It only returns if "quit" is called from another thread
	virtual void run() = 0;
			
	// Suspend the network interface, close everything and return the "run" call
	virtual void quit() = 0;

	// Run the "run" method in a background thread and returns control of the current thread
	void runInBackground();
		
	// Send a packet
	virtual Status send(Packet* packetP) = 0;
		
	// Senda message to all Delilah endpoints
    virtual void delilahSend(Packet* packetP) = 0;

	// Senda message to all Worker endpoints
    virtual void workerSend(Packet* packetP) = 0;
};

}

#endif
