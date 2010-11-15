#include <iostream>				// std::cout ...

#include "logMsg.h"             // lmInit, LM_*

#include "Macros.h"             // EXIT, ...
#include "Message.h"            // Message::MessageCode, ...
#include "Packet.h"				// ss::Packet
#include "Network.h"			// NetworkInterface
#include "Endpoint.h"			// Endpoint
#include "CommandLine.h"		// CommandLine
#include "Delilah.h"			// Own interfce
#include "Packet.h"				// ss::Packet
#include "DelilahConsole.h"		// ss::DelilahConsole
#include "DelilahQt.h"			// DelilahQt
#include "DelilahLoadDataProcess.h"	// ss::DelilahLoadDataProcess

namespace ss {

	void* runNetworkThread(void *p)
	{
		Delilah *d = ((Delilah*)p);
		d->runNetwork();
		return NULL;
	}
	
	Delilah::Delilah( int arg, const char *argv[] , NetworkInterface *_network )
	{
		
		//  Keep command line parameters for QT initizalization
		_argc =  arg;
		_argv = argv;
		
		network = _network;		// Keep a pointer to our network interface element
		network->setPacketReceiverInterface(this);
		
		loadDataCounter = 0;
		
		finish = false;				// Global flag to finish threads
		
		// Parse input command lines
		au::CommandLine commandLine;
		commandLine.set_flag_string("controller", "no_controller");
		
		commandLine.set_flag_boolean("console");				
		commandLine.set_flag_boolean("basic");				// Basic console without ncurses		
		commandLine.set_flag_boolean("r");
		commandLine.set_flag_boolean("w");
		
		commandLine.parse(arg, argv);
		
		// Create console or graphical interface
		
		lmReads    = commandLine.get_flag_bool("r");
		lmWrites   = commandLine.get_flag_bool("w");
		
		if ( commandLine.get_flag_bool("console") )
			client = new DelilahConsole(this,true);		// Console with ncurses
		else if ( commandLine.get_flag_bool("basic") )
			client = new DelilahConsole(this,false);		// Console without ncurses
		else
			client = new DelilahQt( this );
		
		
		// Get the controller
		std::string controller = commandLine.get_flag_string("controller");
		
		if (controller == "no_controller")
			std::cerr  << "Controller has not been specified with command line parameets" << std::endl;
		else
			initController( controller );
	}	
	
	void Delilah::initController( std::string controller )
	{
		// LM_T( TRACE_DELILAH , ("Delilah running. Controller: %s",controller.c_str() ) );
		network->initAsDelilah( controller );
		
		// run network "run" in a separate thread
		pthread_create(&t_network, NULL, runNetworkThread, this);
	}	
	
	void Delilah::runNetwork()
	{
		// Main run_loop to the network interface
		network->run();	
	}
	
	void Delilah::run()
	{
		// Run a console or a graphical interface ( independently if network interface is created or not )
		assert( client );
		client->run(_argc ,_argv );
	}
	
	void Delilah::quit()
	{
		finish = true;
		client->quit();
		network->quit();
	}
	
	int Delilah::receive(int fromId, Message::MessageCode msgCode, Packet* packet)
	{

		// LoadDataConfirmation message are intercepted here
		
		
		if (msgCode == Message::LoadDataResponse )
		{
			size_t		process_id		= packet->message.load_data_response().process_id();
			size_t		file_id			= packet->message.load_data_response().file_id();
			bool		error		= packet->message.load_data_response().error();

			network::File file = packet->message.load_data_response().file();
			
			loadDataLock.lock();
			
			DelilahLoadDataProcess *process = loadProcess.findInMap( process_id );
			assert( process );

			// Notify about this confirmation
			process->notifyDataLoad( file_id , file , error );
			
			if ( process->isUploadFinish() )
			{
				// Send the final packet to the controller notifying about the loading process
				
				Packet p;
				network::LoadDataConfirmation *confirmation	= p.message.mutable_load_data_confirmation();
				process->fillLoadDataConfirmationMessage( confirmation );
				network->send(this, network->controllerGetIdentifier(), Message::LoadDataConfirmation, &p);
				
				// Confirm to the client that everything is ok
				//client->loadDataConfirmation( process );
			}
			
			loadDataLock.unlock();
			
			return 0;
		}
		
		if (msgCode == Message::LoadDataConfirmationResponse )
		{
			
			size_t process_id			= packet->message.load_data_confirmation_response().process_id();

			DelilahLoadDataProcess *process = loadProcess.extractFromMap( process_id );
			assert( process );

			// Notify the information contained in the confirmation response message
			process->notifyLoadDataConfirmationResponse( packet->message.load_data_confirmation_response() );
			
			// Notify to the client to show on scren the result of this load process
			client->loadDataConfirmation( process );
			
			delete process;
			
			return 0;
		}
		
		// Forward the reception of this message to the client
		assert( client );
		client->receive( fromId , msgCode , packet );

		return 0;
	}
	
	void Delilah::notificationSent(size_t id, bool success)
	{
		// Do something
	}

#pragma mark Load data process
	
	size_t Delilah::loadData( std::vector<std::string> fileNames , std::string queue)
	{
		size_t id = loadDataCounter++;
		
		DelilahLoadDataProcess * d = new DelilahLoadDataProcess(this, network->getNumWorkers() ,id,  fileNames , queue );

		loadDataLock.lock();
		loadProcess.insertInMap( id , d );
		loadDataLock.unlock();
		
		d->run();
		
		return id;
	}
	
	
	
}

