/* ****************************************************************************
 *
 * FILE                     SamsonController.h
 *
 * DESCRIPTION				Main class for the worker elemen
 *
 * ***************************************************************************/

#pragma once
#include <iostream>				// std::cout
#include "network.h"			// NetworkInterface
#include "endpoint.h"			// EndPoint
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
			
			int port = commandLine.get_flag_int( "port" );
			std::cout << "Samson controller running at port " << port << std::endl;
			
			// Define the endpoints of the network interface
			ss::EndPoint controllerEndPoint;								// Get the endPoint controller from somewhere
			ss::EndPoint myEndPoint( commandLine.get_flag_int("port") );	// My endpoint using the port in the command line
			
			std::vector<EndPoint> workerEndPoints;
			// Get the list of workers somehoow
			
			network.initAsSamsonController( myEndPoint , workerEndPoints );	
			
		}
		
		// 
		void run();

		// PacketReceiverInterface
		virtual void receive( Packet *p , EndPoint fromEndPoint );
		
		// PacketSenderInterface
		virtual void notificationSent( size_t id , bool success );

		
		
	};
	
}