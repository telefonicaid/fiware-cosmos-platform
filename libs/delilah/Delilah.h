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
#include "traces.h"				// TRACE_DALILAH
#include <set>					// std::set
#include "Lock.h"				// au::Lock
#include "au_map.h"				// au::map
#include "samson/Environment.h"	// ss::Environment

namespace ss {
	

	// Thread method
	void* runNetworkThread(void *p);
	class DelilahClient;
	class DelilahLoadDataProcess;
	
	/**
	   Main class for the samson client element
	 */

	class Delilah : public PacketReceiverInterface, public PacketSenderInterface
	{
		friend class DelilahLoadDataProcess;
		DelilahClient* client;			// Console or GUI to work with delilah

		// Command line parameters ( necessary for QT run method )
		int           _argc;
		const char**  _argv;

		pthread_t     t_network;

		
		// Internal counter for load data operations
		au::Lock   loadDataLock;
		size_t     loadDataCounter;
		
	public:
		
		// Environment properties to be sent in the next job
		Environment environment;	
		
		au::map<size_t,DelilahLoadDataProcess> loadProcess; 
		

		NetworkInterface* network;	// Network interface
		bool              finish;	// Global flag used by all threads to detect to stop
		
	public:
		Delilah(NetworkInterface *_network, int argC, const char* argV[], const char* controller, int workers, int endpoints, bool console, bool basic);
		
		void run();
		void initController(std::string controller);
		void runNetwork();
		void quit();

		// PacketReceiverInterface
		virtual int receive(int fromId, Message::MessageCode msgCode, Packet* packet);

		// PacketSenderInterface
		virtual void notificationSent(size_t id, bool success);
		
		// Load a list of files to a particular queue
		// This created a thread to load this process
		// The id returned is used to compare the callbacks "loadDataConfirmation"
		size_t loadData( std::vector<std::string> fileNames , std::string queue);
		
	};
}

#endif
