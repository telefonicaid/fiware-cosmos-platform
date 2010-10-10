/* ****************************************************************************
 *
 * FILE                     SamsonWorker.h
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
	
	class SamsonWorker : public PacketReceiverInterface , public PacketSenderInterface
	{
		ss::NetworkInterface network;
		
	public:
		SamsonWorker( int arg , const char *argv[] )
		{
			// Parse input command lines
			au::CommandLine commandLine;
			commandLine.parse(arg , argv);
			commandLine.set_flag_int("port", 1234);		 // -port to indicate the local port to start the worker ( 1234 default port )
			commandLine.set_flag_string("controller" , "no_controller");
			commandLine.parse(arg, argv);

			// Get the port
			int port = commandLine.get_flag_int( "port" );
			// Get the controller
			std::string controller = commandLine.get_flag_string( "controller" );

			if( controller == "no_controller" )
			{
				std::cerr  << "Please specify controller direction with -controller server:port" << std::endl;
				exit(0);
			}
			
			std::cout << "Samson worker running at port " << port << " controller: " << controller << std::endl;
			
			// Get the endpoints necessary to start network interface
			ss::EndPoint controllerEndPoint(controller);						// Get the endPoint controller from somewhere
			ss::EndPoint myEndPoint( commandLine.get_flag_int("port") );	// My endpoint using the port in the command line
			
			network.initAsSamsonWorker( myEndPoint , controllerEndPoint );	
			
		}
		
		// Main routine
		void run();

		// PacketReceiverInterface
		virtual void receive( Packet *p , EndPoint fromEndPoint );

		// PacketSenderInterface
		virtual void notificationSent( size_t id , bool success );
		
		
		
	};
	
}