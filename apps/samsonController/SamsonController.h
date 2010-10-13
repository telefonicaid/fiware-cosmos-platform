#ifndef SAMSON_CONTROLLER_H
#define SAMSON_CONTROLLER_H

/* ****************************************************************************
*
* FILE                     SamsonController.h
*
* DESCRIPTION				Main class for the worker elemen
*
*/
#include <iostream>				// std::cout

#include "logMsg.h"             // lmInit, LM_*
#include "traceLevels.h"        // LMT_*

#include "Macros.h"             // EXIT, ...
#include "network.h"			// NetworkInterface
#include "Endpoint.h"			// Endpoint
#include "CommandLine.h"		// au::CommandLine
#include "samsonDirectories.h"	// File to load setup



namespace ss {
	
	/**
	 Main class for the samson worker element
	 */
	
	class SamsonController : public PacketReceiverInterface , public PacketSenderInterface
	{
		std::vector<Endpoint> workerEndPoints;			// Vector of workers from the setup file	
		
		ss::NetworkInterface network;
		
	public:
		SamsonController( int arg , const char *argv[] )
		{
			// Parse input command lines
			au::CommandLine commandLine;
			commandLine.parse(arg , argv);
			commandLine.set_flag_int("port",      SAMSON_CONTROLLER_DEFAULT_PORT);    // -port to indicate the local port to start
			commandLine.set_flag_string("setup",  SAMSON_SETUP_FILE );                // Setup filename
			commandLine.set_flag_string("t",      "0-255");
			commandLine.parse(arg, argv);
			
			LmStatus s;
			if ((s = lmTraceSet((char*) commandLine.get_flag_string("t").c_str())) != LmsOk)
			   EXIT(1, ("lmTraceSet: %s", lmStrerror(s)));
			
			// Load setup
			LM_T(LMT_CONFIG, ("calling loadSetup"));			
			loadSetup(commandLine.get_flag_string("setup") );
			
			// Define the endpoints of the network interface
			ss::Endpoint myEndPoint( commandLine.get_flag_int("port") );	// My endpoint using the port in the command line
			LM_T(LMT_CONFIG, ("workerEndPoints.size: %d", workerEndPoints.size()));

			network.initAsSamsonController( myEndPoint , workerEndPoints );	
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
						workerEndPoints.push_back(Endpoint( c.get_argument( 1 ) ) );
						LM_T(LMT_CONFIG, ("added worker: '%s'", c.get_argument(1).c_str()));
					}
				}					
			}
				  
			
			
			fclose(file);
		}
		
		// PacketReceiverInterface
		virtual void receive( Packet *p , Endpoint* fromEndPoint );
		
		// PacketSenderInterface
		virtual void notificationSent( size_t id , bool success );

		
		
	};
	
}

#endif
