
#ifndef _H_NETWORK_FAKE
#define _H_NETWORK_FAKE

#include <vector>				// std::vector
#include <sstream>				// std::ostringstream

#include "Message.h"            // ss::Message::MessageCode
#include "Delilah.h"			// ss:Delilah
#include "SamsonWorker.h"		// ss::SamsonWorker
#include "SamsonController.h"	// ss:: SasonController
#include "Endpoint.h"			// ss::EndPoint



namespace ss {
	
	class NetworkFakeCenter;
	
	class NetworkFake : public NetworkInterface
	{
	public:
		
		// Pointer to the controller
		NetworkFakeCenter *center;
		int worker_id;							// To know who is me
		PacketReceiverInterface *receiver;
		
		NetworkFake( int _worker_id , NetworkFakeCenter *_center );
		
		// New interface
		// ------------------------------------------------------------------------------------------------
		// Note: All of them will end up being pure virtual functions
		
		// Inform about everything ready to start
		virtual bool ready();                                   
		
		// Init function ( one and only one of them should be called )
		virtual void initAsSamsonController(int port, int num_workers);
		virtual void initAsSamsonWorker(int localPort, const char* alias, const char* controller);
		virtual void initAsDelilah(std::string controllerEndpoint);
		
		// Set the receiver element ( this should be notified about the packaked )
		virtual void setPacketReceiverInterface( PacketReceiverInterface* receiver);
		
		// Get identifiers of known elements
		virtual int controllerGetIdentifier();		// Get the identifier of the controller
		virtual int workerGetIdentifier(int i);		// Get the identifier of the i-th worker
		virtual int getMyidentifier();				// Get my identifier
		virtual int getNumWorkers();					// Get the number of workers
		virtual int getWorkerFromIdentifier( int identifier);	// TODO: convert this to pure virtual when implemented
		
		
		// Send a packet (return a unique id to inform the notifier later)
		virtual size_t send(PacketSenderInterface* sender, int endpointId, ss::Message::MessageCode code, void* data = NULL, int dataLen = 0, Packet* packetP = NULL);
		
		// Main run loop control to the network interface
		virtual void run();           
		
		// Syspend the network interface, close everything and return the "run" call
		virtual void quit();	
	};
	
}


#endif
