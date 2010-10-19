#ifndef SAMSON_WORKER_H
#define SAMSON_WORKER_H

/* ****************************************************************************
*
* FILE                     SamsonWorker.h
*
* DESCRIPTION			   Main class for the worker element
*
*/

#include <iostream>				// std::cout

#include "Network.h"			// NetworkInterface
#include "samsonDirectories.h"  // SAMSON_CONTROLLER_DEFAULT_PORT
#include "Endpoint.h"			// Endpoint
#include "CommandLine.h"		// au::CommandLine
#include "logMsg.h"				// 
#include "Macros.h"				// EXIT
#include "traces.h"				// Trace levels

namespace ss {
	
	/**
	 Main class for the samson worker element
	 */
	
	class SamsonWorker : public PacketReceiverInterface , public PacketSenderInterface
	{
		NetworkInterface* network;
		
	public:
		SamsonWorker(int argc, const char* argv[] , NetworkInterface *_network)
		{
			network = _network;
			network->setPacketReceiverInterface(this);
			
			int          port;
			std::string  controller;
			std::string  trace;
			
			// Parse input command lines
			au::CommandLine commandLine;
			commandLine.parse(argc, argv);

			commandLine.set_flag_int("port",           SAMSON_CONTROLLER_DEFAULT_PORT);
			commandLine.set_flag_string("controller" , "no_controller");
			commandLine.set_flag_string("t",           "255");
			commandLine.set_flag_boolean("r");
			commandLine.set_flag_boolean("w");

			commandLine.parse(argc, argv);

			port       = commandLine.get_flag_int("port");
			controller = commandLine.get_flag_string("controller");
			lmReads    = commandLine.get_flag_bool("r");
			lmWrites   = commandLine.get_flag_bool("w");


			if (controller == "no_controller")
			{
				std::cerr  << "Please specify controller direction with -controller server:port" << std::endl;
				exit(0);
			}
			
			LM_T( TRACE_SAMSON_WORKER , ("Samson worker running at port %d controller: %s", port , controller.c_str() ));
					
			// Get the endpoints necessary to start network interface
			ss::Endpoint controllerEndPoint(Endpoint::Controller, controller);
			ss::Endpoint myEndPoint(Endpoint::Worker, port);
			
			network->initAsSamsonWorker(myEndPoint, controllerEndPoint);
		}
		
		
		// Main routine
		void run();

		// Test routine
		void test();
		
		// PacketReceiverInterface
		virtual void receive(Packet* p, Endpoint* fromEndPoint);

		// PacketSenderInterface
		virtual void notificationSent(size_t id, bool success);
	};
}

#endif
