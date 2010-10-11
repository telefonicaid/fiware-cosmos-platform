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

namespace ss {
	
	/**
	 Main class for the samson worker element
	 */
	
	class SamsonController : public PacketReceiverInterface , public PacketSenderInterface
	{
		ss::NetworkInterface network;
		
	public:
		SamsonController( int arg , const char *argv[] )
		{
			// Parse input command lines
			au::CommandLine commandLine;
			commandLine.parse(arg , argv);
			commandLine.set_flag_int("port", 1235);		 // -port to indicate the local port to start the worker ( 1234 default port )
			commandLine.parse(arg, argv);
						
			// Define the endpoints of the network interface
			ss::Endpoint myEndPoint( commandLine.get_flag_int("port") );	// My endpoint using the port in the command line
			
			// Get the list of workers from the command line
			std::vector<Endpoint> workerEndPoints;
			
			for (int i = 1 ; i < commandLine.get_num_arguments() ; i++)
				workerEndPoints.push_back( Endpoint( commandLine.get_argument( i ) ) );

			network.initAsSamsonController( myEndPoint , workerEndPoints );	
			
			std::cout << "Samson controller running at port " << commandLine.get_flag_int("port") << std::endl;
			std::cout << "List of workers:\n";
			for (size_t i = 0 ; i < workerEndPoints.size() ; i++)
				std::cout << workerEndPoints[i].str() << std::endl;
			
			
		}
		
		// Main run loop
		void run();

		
		void test();
		
		
		// PacketReceiverInterface
		virtual void receive( Packet *p , Endpoint* fromEndPoint );
		
		// PacketSenderInterface
		virtual void notificationSent( size_t id , bool success );

		
		
	};
	
}

#endif
