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
			commandLine.set_flag_int("port", SAMSON_CONTROLLER_DEFAULT_PORT);	// -port to indicate the local port to start
			commandLine.set_flag_string("setup",SAMSON_SETUP_FILE );			// Setup filename
			commandLine.parse(arg, argv);
			
			// Load setup
			loadSetup(  commandLine.get_flag_string("setup") );
			
			// Define the endpoints of the network interface
			ss::Endpoint myEndPoint( commandLine.get_flag_int("port") );	// My endpoint using the port in the command line
			
			network.initAsSamsonController( myEndPoint , workerEndPoints );	
			
			std::cout << "Samson controller running at port " << commandLine.get_flag_int("port") << std::endl;
			std::cout << "List of workers:\n";
			for (size_t i = 0 ; i < workerEndPoints.size() ; i++)
				std::cout << workerEndPoints[i].str() << std::endl;			
			
		}
		
		// Main run loop
		void run();

		
		void test();
		
		
		
		void loadSetup(std::string fileName)
		{
			FILE *file = fopen(fileName.c_str(),"r");
			if( !file )
			{
				LM_W(("Setup file not found %s", SAMSON_SETUP_FILE ));
				return;
			}
			
			workerEndPoints.clear();
			
			char line[2000];
			while( fgets(line, 2000, file) )
			{
				au::CommandLine c;
				c.parse(line);
				
				if( c.get_num_arguments() == 0 )
					continue;

				std::string mainCommand = c.get_argument(0);
				if( mainCommand[0] == '#' )
					continue;
				
				if ( mainCommand == "worker")
					if( c.get_num_arguments()>=2 )	
						workerEndPoints.push_back( Endpoint( c.get_argument( 1 ) ) );
					
				
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
