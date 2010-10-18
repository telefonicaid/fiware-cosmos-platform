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

#define LMT_CONFIG    22


namespace ss {
	
	/**
	 Main class for the samson worker element
	 */
	
	class SamsonController : public PacketReceiverInterface , public PacketSenderInterface
	{
		std::vector<Endpoint> workerEndPoints;			// Vector of workers from the setup file	
		
		NetworkInterface *network;						// Network interface
		ControllerDataManager data;						// Data manager for the controller
		ModulesManager modulesManager;					// Manager of the modules ( to check data types and map/reduce/scripts functions)

		
	public:
		SamsonController( int arg , const char *argv[] ,  NetworkInterface *_network )
		{
			
			network = _network;
			network->setPacketReceiverInterface(this);
			
			int          port;
			std::string  trace;
			std::string  setup;
			
			// Parse input command lines
			au::CommandLine commandLine;
			commandLine.parse(arg , argv);

			commandLine.set_flag_int("port",      SAMSON_CONTROLLER_DEFAULT_PORT);
			commandLine.set_flag_string("setup",  SAMSON_SETUP_FILE );
			commandLine.set_flag_string("t",      "255");
            commandLine.set_flag_boolean("r");
            commandLine.set_flag_boolean("w");

			commandLine.parse(arg, argv);
			
            port       = commandLine.get_flag_int("port");
			setup      = commandLine.get_flag_string("setup");
            trace      = commandLine.get_flag_string("t");
            lmReads    = commandLine.get_flag_bool("r");
            lmWrites   = commandLine.get_flag_bool("w");

			LmStatus s;
            if ((s = lmTraceSet((char*) trace.c_str())) != LmsOk)
				EXIT(1, ("lmTraceSet: %s", lmStrerror(s)));
			
			// Load setup
			LM_T(LMT_CONFIG, ("calling loadSetup"));			
			loadSetup(setup);
			
			// Define the endpoints of the network interface
			ss::Endpoint myEndPoint(Endpoint::Listener, port);

			LM_T(LMT_CONFIG, ("workerEndPoints.size: %d", workerEndPoints.size()));

			network->initAsSamsonController(myEndPoint, workerEndPoints);
		}
		
		
		// Main run loop
		void run();

		
		void test();
		
		
		
		void loadSetup(std::string fileName)
		{
			LM_T(LMT_CONFIG, ("IN"));

			FILE *file = fopen(fileName.c_str(),"r");
			if (!file)
				LM_RVE(("Config file '%s' not found", fileName.c_str()));
			
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
		}
		
		
		
		// Send a message back to dalilah
		void sendDalilahAnswer( size_t sender_id , Endpoint *dalilahEndPoint , bool error , std::string answer_message );
		
		
		// PacketReceiverInterface
		virtual void receive( Packet *p , Endpoint* fromEndPoint );
		
		// PacketSenderInterface
		virtual void notificationSent( size_t id , bool success );

		
		
		
		
	};
	
}

#endif
