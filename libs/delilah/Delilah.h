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
#include "DelilahQt.h"			// DelilahQt


namespace ss {
	

	// Thread method
	void* runNetworkThread(void *p);
	
	/**
	   Main class for the samson client element
	 */

	class Delilah : public PacketReceiverInterface, public PacketSenderInterface
	{
		NetworkInterface* network;			// Network interface

		DelilahConsole* console;			// Console to work with delilah in command-line mode
		DelilahQt *interfaceQt;				// Graphical interface

		// Command line parameters ( necessary for QT run method )
		int _argc;
		const char **_argv;

		pthread_t t_network;

		friend class DelilahConsole;
		friend class DelilahQt;
		
	public:
		bool finish;						// Global flag used by all threads to detect to stop
		
	public:
		
		Delilah( int arg, const char *argv[] , NetworkInterface *_network )
		{
			
			//  Keep command line parameters for QT initizalization
			_argc =  arg;
			_argv = argv;
		
			network = _network;		// Keep a pointer to our network interface element
			network->setPacketReceiverInterface(this);
			
			finish = false;				// Global flag to finish threads
			
			// Parse input command lines
			au::CommandLine commandLine;
			commandLine.set_flag_string("controller", "no_controller");
			
			commandLine.set_flag_boolean("console");				
			commandLine.set_flag_boolean("basic");				// Basic console without ncurses		
			
			commandLine.parse(arg, argv);

			// Create console or graphical interface
			
			console = 0;
			interfaceQt = 0;

			if ( commandLine.get_flag_bool("console") )
				console = new DelilahConsole(this,true);		// Console with ncurses
			else if ( commandLine.get_flag_bool("basic") )
				console = new DelilahConsole(this,false);		// Console without ncurses
			else
				interfaceQt = new DelilahQt( this );
				
			
			// Get the controller
			std::string controller = commandLine.get_flag_string("controller");
			
			if (controller == "no_controller")
			{
				std::cerr  << "Controller has not been specified with command line parameets" << std::endl;
				//exit(0);
			}
			else
				initController( controller );

		}
		
		void run();
		
		void initController( std::string controller )
		{
		  // LM_T( TRACE_DELILAH , ("Delilah running. Controller: %s",controller.c_str() ) );
			network->initAsDelilah( controller );

			// run network "run" in a separate thread
			pthread_create(&t_network, NULL, runNetworkThread, this);
		}
		
		// Run the network run method
		void runNetwork();
		
		void quit()
		{
			finish = true;
			
			if( console )
				console->quit();
			if( interfaceQt )
				interfaceQt->quit();
			
			network->quit();
		}
		
		// PacketReceiverInterface
		virtual int receive(int fromId, Message::MessageCode msgCode, Packet* packet);

		// PacketSenderInterface
		virtual void notificationSent(size_t id, bool success);
	};
}

#endif
