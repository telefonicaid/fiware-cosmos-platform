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
		
		friend class ControllerTaskManager;
		
	public:
		SamsonController( int arg , const char *argv[] ,  NetworkInterface *_network ) : 	taskManager( this )
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
		
		
		// Main run loop
		void run();

		
		void test();
		
		
		std::vector <std::string> getworkerPeers( std::string fileName )
		{
			std::vector <std::string> workerPeers;
			
			LM_T(LMT_CONFIG, ("IN"));
			
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
		
		
		std::vector <Endpoint> loadSetup(std::string fileName)
		{
			std::vector <Endpoint> workerEndPoints;
			
			LM_T(LMT_CONFIG, ("IN"));

			FILE *file = fopen(fileName.c_str(),"r");
			if (!file)
			{
				LM_E(("Config file '%s' not found", fileName.c_str()));
				return workerEndPoints;
			}
			
			workerEndPoints.clear();
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
						workerEndPoints.push_back(Endpoint(Endpoint::Worker, c.get_argument(1)));
						LM_T(LMT_CONFIG, ("added worker: '%s'", c.get_argument(1).c_str()));
					}
				}					
			}
				  
			fclose(file);
			
			return workerEndPoints;
		}
		
		
		// Send a message back to dalilah
		void sendDalilahAnswer( size_t sender_id , int dalilahIdentifier , bool error , bool finished, std::string answer_message );
		
		// Send a message to a worker with a particular task
		void sendWorkerTasks( ControllerTask *task );
		void sendWorkerTask( int  workerIdentifier , size_t task_id , std::string command );
		
		// PacketReceiverInterface
		void receive( Packet *p , int fromIdentifier );
		
		// PacketSenderInterface
		virtual void notificationSent( size_t id , bool success );

		
		
		
		
	};
	
}

#endif
