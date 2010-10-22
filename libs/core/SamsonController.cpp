#include <iostream>				        // std::cout ...

#include "logMsg.h"                     // lmInit, LM_*

#include "Macros.h"                     // EXIT, ...
#include "Packet.h"				        // ss::Packet
#include "Network.h"			        // NetworkInterface
#include "Endpoint.h"			        // EndPoint
#include "CommandLine.h"		        // CommandLine
#include "ModulesManager.h"		        // ss:ModulesManager
#include "ControllerTaskManager.h"		// ss:ControllerTaskManager
#include "ControllerTask.h"				// ss:ControllerTask
#include "SamsonController.h"	        // Own interface ss::SamsonController



namespace ss {

	SamsonController::SamsonController( int arg , const char *argv[] ,  NetworkInterface *_network ) : 	taskManager( this )
	{
		
		std::string  setupFileName;						// Filename with setup information
		int          port;								// Local port where this controller listen
		
		
		network = _network;
		network->setPacketReceiverInterface(this);
		
		std::string  trace;
		
		// Parse input command lines
		au::CommandLine commandLine;
		commandLine.parse(arg , argv);
		
		commandLine.set_flag_int("port",      SAMSON_CONTROLLER_DEFAULT_PORT);
		commandLine.set_flag_string("setup",  SAMSON_SETUP_FILE );
		commandLine.set_flag_string("t",      "255");
		commandLine.set_flag_boolean("r");
		commandLine.set_flag_boolean("w");
		
		commandLine.parse(arg, argv);
		
		port			= commandLine.get_flag_int("port");
		setupFileName   = commandLine.get_flag_string("setup");
		lmReads			= commandLine.get_flag_bool("r");
		lmWrites		= commandLine.get_flag_bool("w");
		
		
		// Load setup
		LM_T(LMT_CONFIG, ("calling loadSetup"));			
		//std::vector <Endpoint> workerEndPoints = loadSetup(setupFileName);
		std::vector <std::string> workerPeers = getworkerPeers(setupFileName);
		
		// Define the endpoints of the network interface
		
		LM_T(LMT_CONFIG, ("workerEndPoints.size: %d", workerPeers.size()));
		network->initAsSamsonController(port, workerPeers);
		//network->initAsSamsonController(myEndPoint, workerEndPoints);
	}	
	
	void SamsonController::run()
	{
		data.initDataManager( data.getLogFileName() );			// Init the data manager
		network->run();											// Run the network interface (blocked)
	}
	
	void SamsonController::receive( Packet *p , int fromIdentifier )
	{
		
		if( p->message.type() == network::Message_Type_Command )
		{
			
			au::CommandLine cmdLine;
			cmdLine.parse( p->message.command().command() );
			std::ostringstream output;					// General output string buffer
			
			if( cmdLine.get_num_arguments() == 0)
				return;
			
			// General status command
			if( cmdLine.get_argument(0) == "status" )
			{
				output << "Status of controller" << std::endl;			
				output << "====================" << std::endl;
				output << data.status();
				output << taskManager.status();

				// Information about each server
				for (int i = 0 ; i < network->getNumWorkers() ; i++)
				{
					output << "Worker " << i << std::endl;
					output << "\tCores " << status[i].system().cores_running() << "/" << status[i].system().cores_total() << std::endl;
				}
				
				// Get status of controller
				sendDalilahAnswer( p->message.command().sender_id() , fromIdentifier , false , true, output.str() );
				return;
			}
			
			// Try to schedule the command
			bool success = taskManager.addTask( fromIdentifier ,  p->message.command().command() , output  );
			
			// Send something back to dalilah ( if error -> it is also finish )
			sendDalilahAnswer( p->message.command().sender_id() , fromIdentifier , !success , !success,  output.str() );
			
			return;
		}
		
		// A confirmation from a worker is received
		
		if( p->message.type() == network::Message_Type_WorkerTaskConfirmation )
		{
			taskManager.notifyWorkerConfirmation( fromIdentifier , p->message.worker_task_confirmation() );			
			return;
		}
		
		
		// Status update from a worker
		if( p->message.type() == network::Message_Type_WorkerStatus )
		{
			int worker = network->getWorkerFromIdentifier( fromIdentifier );
			
			if( worker != -1 )	// Valid worker
				status[worker] = p->message.worker_status();
			
		}
		
	}

	void SamsonController::notificationSent( size_t id , bool success )
	{
		// Do something
	}
	
	std::vector <std::string> SamsonController::getworkerPeers( std::string fileName )
	{
		std::vector <std::string> workerPeers;
		
		FILE *file = fopen(fileName.c_str(),"r");
		if (!file)
		{
			LM_E(("Config file '%s' not found", fileName.c_str()));
			return workerPeers;
		}
		
		workerPeers.clear();
		char line[2000];
		while( fgets(line, sizeof(line), file))
		{
			au::CommandLine c;
			c.parse(line);
			
			if( c.get_num_arguments() == 0 )
				continue;
			
			std::string mainCommand = c.get_argument(0);
			if( mainCommand[0] == '#' )
				continue;
			
			if (mainCommand == "worker")
			{
				if (c.get_num_arguments() >= 2)	
				{
					workerPeers.push_back(c.get_argument(1) );;
					LM_T(LMT_CONFIG, ("added worker: '%s'", c.get_argument(1).c_str()));
				}
			}					
		}
		
		fclose(file);
		
		return workerPeers;
	}
	
	
#pragma mark Sent messages
	
	
	void SamsonController::sendDalilahAnswer( size_t sender_id , int dalilahIdentifier , bool error , bool finished, std::string answer_message )
	{
		// Get status of controller
		Packet p2( network::Message_Type_CommandResponse );
		
		network::CommandResponse *response = p2.message.mutable_command_response();
		response->set_response( answer_message );
		response->set_error( error );
		response->set_finish( finished );
		response->set_sender_id( sender_id );
		
		network->send(&p2, dalilahIdentifier , this);
	}
	
	void SamsonController::sendWorkerTasks( ControllerTask *task )
	{
		// Send messages to the workers indicating the operation to do ( waiting the confirmation from all of them )
		for (int i = 0 ; i < network->getNumWorkers() ; i++)
			sendWorkerTask( i , task->getId() , task->getCommand() );
	}	
	
	void SamsonController::sendWorkerTask( int workerIdentifier , size_t task_id , std::string command )
	{
		
		// Get status of controller
		Packet p2( network::Message_Type_WorkerTask );
		
		network::WorkerTask *t = p2.message.mutable_worker_task();
		t->set_command( command );
		t->set_task_id( task_id );
		
		network->send(&p2, workerIdentifier, this);
	}
	
	
	
	
	
	
	
}
