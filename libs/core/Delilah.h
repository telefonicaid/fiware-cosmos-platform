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

#include "logMsg.h"             // lmInit, LM_*

#include "Macros.h"             // EXIT, ...
#include "Network.h"			// NetworkInterface
#include "Message.h"            // Message::MessageCode
#include "Endpoint.h"			// Endpoint
#include "CommandLine.h"		// au::CommandLine
#include "CommandLine.h"		// au::CommandLine
#include "DelilahConsole.h"		// ss::DelilahConsole
#include "traces.h"				// TRACE_DALILAH



namespace ss {
	

	
	/**
	   Main class for the samson client element
	 */

	class Delilah : public PacketReceiverInterface, public PacketSenderInterface
	{
		NetworkInterface* network;			// Network interface
		DelilahConsole* console;			// Console to work with delilah in command-line mode

		size_t	local_id;					// Internal counter to keep count of the commands sent to the controller
		
	public:
		bool finish;						// Global flag used by all threads to detect to stop
		
	public:
		
		Delilah( int arg, const char *argv[] ,NetworkInterface *_network )
		{
			network = _network;			// Keep a pointer to our network interface element
			network->setPacketReceiverInterface(this);
			
			finish = false;				// Global flag to finish threads
			local_id = 0;				// Init the counter of messages sent to the controller
			
			// Parse input command lines
			au::CommandLine commandLine;
			commandLine.set_flag_string("controller", "no_controller");
			commandLine.set_flag_boolean("console");				
			
			commandLine.parse(arg, argv);

			// Create console
			console = new DelilahConsole(this, !commandLine.get_flag_bool("console"));
			
			// Get the controller
			std::string controller = commandLine.get_flag_string("controller");
			
			if (controller == "no_controller")
			{
				std::cerr  << "Please specify controller direction with -controller server:port" << std::endl;
				exit(0);
			}

			LM_T( TRACE_DALILAH , ("Delilah running. Controller: %s",controller.c_str() ) );
			
			//ss::Endpoint controllerEndpoint(Endpoint::Controller, controller);
			//network->initAsDelilah(controllerEndpoint);
			network->initAsDelilah(controller);
		}
		
		
		// Main routine
		void run();

		// Run console
		void runConsole()
		{
			console->run();
		}
		
		void quit()
		{
			finish = true;
			console->quit();
			network->quit();
		}
		
		void showMessage(std::string m )
		{
			//Show a particular message on screen
			console->writeOnConsole(m);
		}
		
		/**
		 Basic interface to send and receive messages
		 */
		
		size_t sendMessageToController(std::string message);
		int receivedMessage( size_t id , bool error , bool finished , std::string message );
		
		
		
		// PacketReceiverInterface
		virtual int receive(int fromId, Message::MessageCode msgCode, void* dataP, int dataLen, Packet* packet);

		// PacketSenderInterface
		virtual void notificationSent(size_t id, bool success);
	};
}

#endif
