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



/* ****************************************************************************
*
* runNetworkThread - 
*/
void* runNetworkThread(void *p)
{
	Delilah *d = ((Delilah*)p);
	d->runNetwork();
	return NULL;
}
	


/* ****************************************************************************
*
* Delilah::Delilah
*/
Delilah::Delilah
(
	NetworkInterface* _network,
	int               argC,
	const char*       argV[],
	const char*       controller,
	int               workers,
	int               endpoints,
	bool              console,
	bool              basic
)
{
	//  Keep command line parameters for QT initizalization
	_argc = argC;
	_argv = argV;
		
	network = _network;		// Keep a pointer to our network interface element
	network->setPacketReceiverInterface(this);
		
	loadDataCounter = 0;
		
	finish = false;				// Global flag to finish threads
		
	if (basic && console)
		LM_X(1, ("cannot run in basic mode and console mode at a time ..."));

	if (console)
		client = new DelilahConsole(this, true);   // Console with ncurses
	else if (basic)
		client = new DelilahConsole(this, false);  // Console without ncurses
	else
		client = new DelilahQt(this);
		
	if (strcmp(controller, "no_controller") == 0)
		LM_W(("controller not specified as command line parameter"));
	else
		initController(controller);
}
	


/* ****************************************************************************
*
* initController -  
*/
void Delilah::initController(std::string controller)
{
	network->init(Endpoint::Delilah, "delilah", 0, controller.c_str());

	// run network "run" in a separate thread
	pthread_create(&t_network, NULL, runNetworkThread, this);
}	
	


/* ****************************************************************************
*
* runNetwork - 
*/
void Delilah::runNetwork()
{
	// Main run_loop to the network interface
	network->run();	
}
	


/* ****************************************************************************
*
* run - 
*/
void Delilah::run()
{
	// Run a console or a graphical interface ( independently if network interface is created or not )
	assert( client );
	client->run(_argc ,_argv );
}
	


/* ****************************************************************************
*
* quit - 
*/
void Delilah::quit()
{
	finish = true;
	client->quit();
	network->quit();
}
	


/* ****************************************************************************
*
* receive - 
*/
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
				
			Packet *p = new Packet();
			network::LoadDataConfirmation *confirmation	= p->message.mutable_load_data_confirmation();
			process->fillLoadDataConfirmationMessage( confirmation );
			network->send(this, network->controllerGetIdentifier(), Message::LoadDataConfirmation, p);
				
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
	


/* ****************************************************************************
*
* notificationSent - 
*/
void Delilah::notificationSent(size_t id, bool success)
{
	// Do something
}



#pragma mark Load data process
	


/* ****************************************************************************
*
* loadData - 
*/
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

