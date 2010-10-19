#ifndef _H_NETWORK_INTERFACE
#define _H_NETWORK_INTERFACE

namespace ss {

	class Packet;
	
	/* ****************************************************************************
	 *
	 * PacketReceiverInterface - 
	 */
	class PacketReceiverInterface
	{
	public:
		
		// Notify that a nessage has been received
		virtual void receive(Packet* packet, int from)=0;
		
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

		
		
		// New interface
		// ------------------------------------------------------------------------------------------------
		// Note: All of them will end up being pure virtual functions
		
		// Inform about everything ready to start
		virtual bool ready()=0;                                   
		
		// Init function ( one and only one of them should be called )
		virtual void initAsSamsonController(int port, std::vector<std::string> peers)=0;
		virtual void initAsSamsonWorker(int localPort, std::string controllerEndpoint)=0;
		virtual void initAsDelilah(std::string controllerEndpoint)=0;

		// Set the receiver element ( this should be notified about the packaked )
		virtual void setPacketReceiverInterface( PacketReceiverInterface* receiver)=0;

		// Get identifiers of known elements
		virtual int controllerGetIdentifier()=0;		// Get the identifier of the controller
		virtual int workerGetIdentifier(int i)=0;		// Get the identifier of the i-th worker
		virtual int getMyidentifier()=0;				// Get my identifier
		virtual int getNumWorkers()=0;					// Get the number of workers
		
		// Send a packet (return a unique id to inform the notifier later)
		virtual size_t send(Packet* packet, int toIdentifier, PacketSenderInterface* sender)=0;

		// Main run loop control to the network interface
		virtual void run()=0;           
		
		// Syspend the network interface, close everything and return the "run" call
		virtual void quit()=0;					
		
		
		
		// Old interface methods ( to be removed )
		// ------------------------------------------------------------------------------------------------
		/*
		virtual void initAsSamsonController(Endpoint myEndpoint, std::vector<Endpoint> peers) = 0;
		virtual void initAsSamsonWorker(Endpoint myEndpoint, Endpoint controllerEndpoint) = 0;
		virtual void initAsDelilah(Endpoint controllerEndpoint) = 0;
		
		// Information about identifiers
		virtual Endpoint* meGet()=0;                              // Get my endPoint
		virtual Endpoint* controllerGet()=0;                      // Get the endPoint of the controller
		virtual Endpoint* workerGet(int i)=0;                     // Get the endPoint of the "i-th" worker
		virtual int worker( Endpoint* endPoint )=0;               // Identify the worker of this endPoint ( 0 if the controller )

		virtual std::vector<Endpoint*> endPoints()=0;              // Get a list of all endPoints
		virtual std::vector<Endpoint*> samsonWorkerEndpoints()=0;  // Get a list of the samsonWorkers endpoints
		
		// Send a packet (return a unique id to inform the notifier later)
		virtual size_t send(Packet* packet, Endpoint* endpoint, PacketSenderInterface* sender)=0;
		*/

	};
	
	
}

#endif
