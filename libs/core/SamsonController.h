#ifndef SAMSON_CONTROLLER_H
#define SAMSON_CONTROLLER_H

/* ****************************************************************************
*
* FILE                     SamsonController.h
*
* DESCRIPTION				Main class for the worker elemen
*
*/
#include <iostream>						// std::cout
#include "logMsg.h"						// lmInit, LM_*
#include "Macros.h"						// EXIT, ...
#include "Network.h"					// NetworkInterface
#include "Endpoint.h"					// Endpoint
#include "CommandLine.h"				// au::CommandLine
#include "samsonDirectories.h"			// File to load setup
#include "ControllerDataManager.h"		// ss::ControllerDataManager
#include "ModulesManager.h"				// ss::ModulesManager
#include "ControllerTaskManager.h"		// ss::ControllerTaskManager
#include "traces.h"						// LMT_CONFIG, ...
#include "samson.pb.h"					// network::...

//#define LMT_CONFIG    22


namespace ss {
	
	/**
	 Main class for the samson worker element
	 */
	
	class SamsonController : public PacketReceiverInterface , public PacketSenderInterface
	{

		// Elements inside the SamsonController
		
		NetworkInterface*     network;					// Network interface
		ControllerDataManager data;						// Data manager for the controller
		ModulesManager        modulesManager;			// Manager of the modules ( to check data types and map/reduce/scripts functions)
		ControllerTaskManager taskManager;				// Task manager of the controller
		
		// Status information of the workers
		network::WorkerStatus status[ 100 ];			// Status update from all workers
		
		friend class ControllerTaskManager;
		
	public:
		SamsonController( int arg , const char *argv[] ,  NetworkInterface *_network );
		
		// Main run loop
		void run();
		
		// PacketReceiverInterface
		void receive( Packet *p , int fromIdentifier );
		
		// PacketSenderInterface
		virtual void notificationSent( size_t id , bool success );

	private:
		
		// Send a message back to dalilah
		void sendDalilahAnswer( size_t sender_id , int dalilahIdentifier , bool error , bool finished, std::string answer_message );
		
		// Send a message to a worker with a particular task
		void sendWorkerTasks( ControllerTask *task );
		void sendWorkerTask( int  workerIdentifier , size_t task_id , std::string command );
		
		
		// Get the list of workers from the setup file
		std::vector <std::string> getworkerPeers( std::string fileName );
		
		
	};
	
}

#endif
