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
#include "network.h"			// NetworkInterface
#include "Endpoint.h"			// Endpoint
#include "CommandLine.h"		// au::CommandLine


namespace ss {
	
	/**
	   Main class for the samson client element
	 */

	class Delilah : public PacketReceiverInterface, public PacketSenderInterface
	{
		ss::NetworkInterface network;
		
	public:
		
		Delilah( int arg, const char *argv[] )
		{
			// Parse input command lines
			au::CommandLine commandLine;
			commandLine.set_flag_string("controller", "no_controller");
			commandLine.parse(arg, argv);
			
			// Get the controller
			std::string controller = commandLine.get_flag_string("controller");
			
			if (controller == "no_controller")
			{
				std::cerr  << "Please specify controller direction with -controller server:port" << std::endl;
				exit(0);
			}

			std::cout << "Delaila running. Controller: " << controller << std::endl;
			
			ss::Endpoint controllerEndpoint(controller);		// Get the endPoint controller from somewhere
			network.initAsDelailah(controllerEndpoint);
			
		}
		
		// Main routine
		void run();

		// Simple test sending packets
		void test();
		
		// PacketReceiverInterface
		virtual void receive(Packet* p, Endpoint* fromEndpoint);

		// PacketSenderInterface
		virtual void notificationSent(size_t id, bool success);
	};
}

#endif
