#ifndef DELILAH_H
#define DELILAH_H

/* ****************************************************************************
*
* FILE                     Delilah.h
*
* DESCRIPTION			   Client application for Samson
*
*/
#include <iostream>				// std::cout

#include "logMsg.h"             // lmInit, LM_*

#include "Macros.h"             // EXIT, ...
#include "Network.h"			// NetworkInterface
#include "Message.h"            // Message::MessageCode
#include "Endpoint.h"			// Endpoint
#include "traces.h"				// TRACE_DALILAH
#include <set>					// std::set
#include "Lock.h"				// au::Lock
#include "au_map.h"				// au::map
#include "samson/Environment.h"	// ss::Environment
#include "samson.pb.h"			// ss::network::..
#include "au_map.h"				// au::simple_map

namespace ss {
	

	// Thread method
	void* runNetworkThread(void *p);
	class DelilahClient;
	class DelilahComponent;
	
	/**
	   Main class for the samson client element
	 */

	class Delilah : public PacketReceiverInterface, public PacketSenderInterface
	{
		
	public:
		
		DelilahClient* client;			// Console or GUI to work with delilah

		pthread_t     t_network;
		
		au::Lock   lock;										// Internal counter for processing packets
		size_t id;												// Id counter of the command - messages sent to controller ( commands / upload/ download )
		Environment environment;								// Environment properties to be sent in the next job
		au::map<size_t , DelilahComponent> components;			// Map of components that intercept messages

		NetworkInterface* network;								// Network interface
		bool              finish;								// Global flag used by all threads to detect to stop
		
	public:
		
		Delilah( NetworkInterface *_network );
		
		void initController(std::string controller);
		void quit();

		// PacketReceiverInterface
		virtual int receive(int fromId, Message::MessageCode msgCode, Packet* packet);

		// PacketSenderInterface
		virtual void notificationSent(size_t id, bool success);

		// Add particular process that will take input parameters
		size_t addUploadData( std::vector<std::string> fileNames , std::string queue);
		size_t addDownloadProcess( std::string queue , std::string fileName , bool show_on_screen );
		size_t sendCommand( std::string command );
		
		
	private:		
		
		size_t addComponent( DelilahComponent* component );
		
	};
}

#endif
