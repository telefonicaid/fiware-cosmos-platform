/* ****************************************************************************
 *
 * FILE                     network.h - Definition for the network interface
 *
 * ***************************************************************************/

#pragma once

#include <cstring>		// size_t
#include <vector>		// vector

namespace ss {

	
	class EndPoint;
	class Packet;
	
	class PacketReceiverInterface
	{
	public:
		// Notification that a particular sent finished
		virtual void receive( Packet *p , EndPoint fromEndPoint )=0;
	};

	/**
	 Interface for a packet sender that would like to receive notification
	 when the packet is delivered
	 */
	
	class PacketSenderInterface
	{
	public:
		// Notification that the packet has been sent
		virtual void notificationSent( size_t id , bool success )=0;
	};
	
	/**
	 Main Element to interact between samson_controller samson and delailah over the network 
	 */
	
	class NetworkInterface
	{
		PacketReceiverInterface *receiver;
		
	public:
		
		// Constructor
		NetworkInterface(  );
		
		// Init the receiver to get packets
		void setPacketReceiver( PacketReceiverInterface *receiver );
		
		// Different init modes ( only one is executed )
		void initAsSamsonController( EndPoint myEndPoint , std::vector<EndPoint> peers );	
		void initAsSamsonWorker(EndPoint myEndPoint ,  EndPoint controllerEndPoint );
		void initAsDelailah( EndPoint controllerEndPoint );

		
		// Services
		
		bool ready();									// Inform about everything ready
														// The controller expects all the workers to be connected
														// The worker expects to be connetced with all the workers and the controller
														// Delailah expects to be connected with all the workers and the contorller
		
		EndPoint me();									// Get my endPoint
		EndPoint controller();							// Get the endPoint of the controller
		EndPoint worker( int i );						// Get the endPoint of the "i-th" worker
		
		int worker( EndPoint endPoint );				// Identify the worker of this endPoint ( 0 if the controller )
		
		std::vector<EndPoint> endPoints();				// Get a list of all endPoints
		std::vector<EndPoint> samsonWorkersEndPoints();	// Get a list of the samsonWorkers endPoints
		
		// Sent a packet ( return a unique id to inform the notifier latter )
		size_t send( Packet *p , EndPoint endPoint , PacketSenderInterface *sender );

		/**
		 This function is expected to return only if quit() function is called
		 */
		void run();										// Main run loop control to the network interface

		// Syspend the network interface, close everything and return the "run" call
		void quit();
		
	};

	
}