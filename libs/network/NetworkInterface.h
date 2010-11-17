#ifndef _H_NETWORK_INTERFACE
#define _H_NETWORK_INTERFACE

/* ****************************************************************************
*
* FILE                     NetworkInterface.h - network interface
*
*/
#include "Message.h"            // ss::Message::MessageCode
#include "Endpoint.h"           // ss::Endpoint::Type
#include "assert.h"				// assert(.)


namespace ss {



	class Packet;
	
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
		virtual int receive(int fromId, Message::MessageCode msgCode, Packet* packet) = 0;
		
		// Notify that a worker has died 
		virtual void notifyWorkerDied( int worker )
		{
			// This call has to be overwritten by SamsonController
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
	public:
		virtual ~NetworkInterface() {};
		
		// Inform about everything ready to start
		virtual bool ready()=0;                                   
		
		virtual void init(Endpoint::Type type, const char* alias, unsigned short port = 0, const char* controllerName = NULL) {};
		// Init function ( one and only one of them should be called )
		virtual void initAsSamsonController(int port, int num_workers)=0;
		virtual void initAsDelilah(std::string controllerEndpoint)=0;

		// Set the receiver element ( this should be notified about the packaked )
		virtual void setPacketReceiverInterface( PacketReceiverInterface* receiver)=0;

		// Get identifiers of known elements
		virtual int controllerGetIdentifier()=0;		// Get the identifier of the controller
		virtual int workerGetIdentifier(int i)=0;		// Get the identifier of the i-th worker
		virtual int getMyidentifier()=0;				// Get my identifier
		virtual int getNumWorkers()=0;					// Get the number of workers
		virtual int getNumEndpoints() { return 0; }                // Get the number of endpoints

		// Ken: No EndPoint outside NetworkInterface ;)
//		virtual Endpoint* endpointLookup(int ix) { return NULL; }  // Get the ixth endpoint

		// Get the "worker cardinal" from the idenfitier
		// This method should return a value between 0 and (num_workers-1) or -1 if the identifier provided is not related to any workers
		virtual int getWorkerFromIdentifier( int identifier) = 0;
		
		// Send a packet (return a unique id to inform the notifier later)
		virtual size_t send(PacketSenderInterface* sender, int endpointId, ss::Message::MessageCode code, Packet* packetP ) = 0;

		// Main run loop control to the network interface
		virtual void run()=0;           
		
		// Syspend the network interface, close everything and return the "run" call
		virtual void quit()=0;					

	};
	
	
}

#endif
