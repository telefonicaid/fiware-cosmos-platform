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
#include "traceLevels.h"        // LMT_*
#include "Macros.h"             // EXIT, ...
#include "network.h"			// NetworkInterface
#include "Endpoint.h"			// Endpoint
#include "CommandLine.h"		// au::CommandLine
#include "CommandLine.h"		// au::CommandLine
#include "DelilahConsole.h"		// ss::DelilahConsole

namespace ss {
	

	
	/**
	   Main class for the samson client element
	 */

	class Delilah : public PacketReceiverInterface, public PacketSenderInterface
	{
		ss::NetworkInterface network;		// Network interface
		DelilahConsole* console;			// Console to work with delilah in command-line mode

		size_t	local_id;					// Internal counter to keep count of the commands sent to the controller
		
	public:
		bool finish;						// Global flag used by all threads to detect to stop
		bool testFlag;							// Flag to test ( debug only (
		
	public:
		
		Delilah( int arg, const char *argv[] )
		{
			finish = false;				// Global flag to finish threads
			local_id = 0;				// Init the counter of messages sent to the controller
			
			// Parse input command lines
			au::CommandLine commandLine;
			commandLine.set_flag_string("controller", "no_controller");
			commandLine.set_flag_string("t",      "0-255");
			commandLine.set_flag_boolean("test");				// tmp flag for testing
			commandLine.set_flag_boolean("console");				// tmp flag for testing
			commandLine.parse(arg, argv);
			
			LmStatus s;
			if ((s = lmTraceSet((char*) commandLine.get_flag_string("t").c_str())) != LmsOk)
				EXIT(1, ("lmTraceSet: %s", lmStrerror(s)));

			// Test flag for debuggin
			testFlag = commandLine.get_flag_bool("test");
			
			
			// Create console
			console =  new DelilahConsole( this , !commandLine.get_flag_bool("console") );
			
			
			// Get the controller
			std::string controller = commandLine.get_flag_string("controller");
			
			if (controller == "no_controller")
			{
				std::cerr  << "Please specify controller direction with -controller server:port" << std::endl;
				exit(0);
			}

			std::cout << "Delilah running. Controller: " << controller << std::endl;
			
			ss::Endpoint controllerEndpoint(Endpoint::Controller, controller);
			if (!testFlag)
				network.initAsDelilah(controllerEndpoint);
			
		}
		
		// Main routine
		void run();

		// Simple test sending packets
		void test();
		
		// Run console
		void runConsole()
		{
			console->run();
		}
		
		void quit()
		{
			finish = true;
			console->quit();
			network.quit();
		}
		
		void showMessage(std::string m )
		{
			//Show a particular message on screen
			console->writeOnConsole(m);
		}
		
		/**
		 Basic interface to send and receive messages
		 */
		
		size_t sendMessageToContorller(std::string message);
		void receivedMessage( size_t id , std::string message );
		
		
		
		// PacketReceiverInterface
		virtual void receive(Packet* p, Endpoint* fromEndpoint);

		// PacketSenderInterface
		virtual void notificationSent(size_t id, bool success);
	};
}

#endif
