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
#include "samson/Environment.h"	                // ss::Environment
#include "samson.pb.h"			        // ss::network::..
#include "au_map.h"				// au::simple_map

namespace ss {
	
	// Thread method
	class DelilahClient;
	class DelilahComponent;
	class DelilahUploadDataProcess;
	
	/**
	   Main class for the samson client element
	 */

	class Delilah : public PacketReceiverInterface, public PacketSenderInterface
	{
		
	public:
		
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
		size_t addUploadData( std::vector<std::string> fileNames , std::string queue , bool compresion, int _max_num_threads);
		size_t addDownloadProcess( std::string queue , std::string fileName , bool show_on_screen );
		size_t sendCommand( std::string command );
		
		
		
	public:
				
		/** 
		 Methonds implemented by subclasses
		 */
		
		// Function to be implemented by sub-classes to process packets ( not handled by this class )
		virtual int _receive(int fromId, Message::MessageCode msgCode, Packet* packet)=0;

		// A load data process has finished
		virtual void loadDataConfirmation( DelilahUploadDataProcess *process )=0;
		
		// Write something on screen
		virtual void showMessage(std::string message)=0;
		
		// Callback to notify that a particular operation has finished
		virtual void notifyFinishOperation( size_t )=0;
		
	private:		
		
		size_t addComponent( DelilahComponent* component );
		
	};
}

#endif
