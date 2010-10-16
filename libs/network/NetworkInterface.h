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
	 * NetworkInterface - interface of the interconnection element ( Network and NetworkSimulator )
	 */
	
	class  NetworkInterface
	{
	public:
		
		virtual void initAsSamsonController(Endpoint myEndpoint, std::vector<Endpoint> peers)=0;
		virtual void initAsSamsonWorker(Endpoint myEndpoint, Endpoint controllerEndpoint)=0;
		virtual void initAsDelilah(Endpoint controllerEndpoint)=0;
		
		// Set the receiver element
		virtual void setPacketReceiverInterface( PacketReceiverInterface* receiver){};
		
		virtual bool ready()=0;                                   // Inform about everything ready
		
		virtual Endpoint* meGet()=0;                              // Get my endPoint
		virtual Endpoint* controllerGet()=0;                      // Get the endPoint of the controller
		virtual Endpoint* workerGet(int i)=0;                     // Get the endPoint of the "i-th" worker
		virtual int worker( Endpoint endPoint )=0;                // Identify the worker of this endPoint ( 0 if the controller )
		
		virtual std::vector<Endpoint> endPoints()=0;              // Get a list of all endPoints
		virtual std::vector<Endpoint> samsonWorkerEndpoints()=0;  // Get a list of the samsonWorkers endpoints
		
		// Send a packet (return a unique id to inform the notifier later)
		virtual size_t send(Packet* packet, Endpoint* endpoint, PacketSenderInterface* sender)=0;
		
		virtual void run()=0;           // Main run loop control to the network interface
		virtual void quit()=0;			// Syspend the network interface, close everything and return the "run" call

	};
	
	
}

#endif