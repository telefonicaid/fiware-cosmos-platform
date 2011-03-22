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
#include "Token.h"				// au::Lock
#include "au_map.h"				// au::map
#include "samson/Environment.h"	                // ss::Environment
#include "samson.pb.h"			        // ss::network::..
#include "au_map.h"				// au::simple_map

namespace ss {
	
	// Thread method
	class DelilahClient;
	class DelilahComponent;
	class DelilahUploadDataProcess;
	class DelilahDownloadDataProcess;

	extern au::Lock list_lock;						// Lock to protect the list of information
	extern network::OperationList *ol;
	extern network::QueueList *ql;
	
	
	/**
	   Main class for the samson client element
	 */

	class Delilah : public PacketReceiverInterface, public PacketSenderInterface
	{
		// Id counter of the command - messages sent to controller ( commands / upload/ download )
		size_t id;												
		
		// Private token to protect the local list of components
		au::Token token;
		// Map of components that intercept messages
		au::map<size_t , DelilahComponent> components;			
		
	public:
		
		Environment environment;								// Environment properties to be sent in the next job

		NetworkInterface* network;								// Network interface
		bool              finish;								// Global flag used by all threads to detect to stop

		
		// Full information everything in samson controller ( updated with updateInfo() )
		
	public:
		
		Delilah( NetworkInterface *_network ,  bool automatic_update );
		
		void initController(std::string controller);
		void quit();

		// PacketReceiverInterface
		void receive( Packet* packet );

		// PacketSenderInterface
		virtual void notificationSent(size_t id, bool success);

		// Add particular process that will take input parameters
		size_t addUploadData( std::vector<std::string> fileNames , std::string queue , bool compresion, int _max_num_threads);
		size_t addDownloadProcess( std::string queue , std::string fileName , bool show_on_screen );
		size_t sendCommand( std::string command );
		
		
		// Check a particular if
		bool isActive( size_t id );
		
	public:
				
		/** 
		 Methonds implemented by subclasses
		 */
		
		// Function to be implemented by sub-classes to process packets ( not handled by this class )
		virtual int _receive(int fromId, Message::MessageCode msgCode, Packet* packet)=0;

		// A load data process has finished
		virtual void uploadDataConfirmation( DelilahUploadDataProcess *process ){};

		// A download process has finished
		virtual void downloadDataConfirmation( DelilahDownloadDataProcess *process ){};
		
		// Write something on screen
		virtual void showMessage( std::string message)=0;

		// Show traces  ( by default it does nothing )
		virtual void showTrace( std::string message){};
		
		// Callback to notify that a particular operation has finished
		virtual void notifyFinishOperation( size_t )=0;
		
		// Get info about the list of loads
		std::string getListOfLoads();
		
	protected:
		
		void clearComponents();
		
	private:		
		
		size_t addComponent( DelilahComponent* component );
		
	};
}

#endif
