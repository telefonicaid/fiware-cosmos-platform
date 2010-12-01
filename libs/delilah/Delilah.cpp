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
#include "DelilahUploadDataProcess.h"	// ss::DelilahLoadDataProcess
#include "DelilahDownloadDataProcess.h"	// ss::DelilahLoadDataProcess
#include "EnvironmentOperations.h"


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
		
	id = 1;	// we start with process 1 because 0 is no process
		
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
	
	lock.lock();
	
	size_t sender_id = packet->message.delilah_id();
	DelilahComponent *component = components.findInMap( sender_id );
	
	if ( component )
	{
		component->receive( fromId, msgCode, packet );
		
		if ( component->component_finished )
		{
			component = components.extractFromMap(sender_id);
			client->notifyFinishOperation( sender_id );
			delete component;
		}
		
	}
	
	lock.unlock();
	
	if( !component )
	{
		// Forward the reception of this message to the client
		assert( client );
		client->receive( fromId , msgCode , packet );
	}

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
	size_t Delilah::addUploadData( std::vector<std::string> fileNames , std::string queue)
	{
		DelilahUploadDataProcess * d = new DelilahUploadDataProcess( fileNames , queue );
		size_t tmp_id = addComponent(d);	
		d->run();
			
		return tmp_id;
	}
	
	/* ****************************************************************************
	 *
	 * download data - 
	 */
	
	
	size_t Delilah::addDownloadProcess( std::string queue , std::string fileName )
	{
		DelilahDownloadDataProcess *d = new DelilahDownloadDataProcess( queue , fileName );
		size_t tmp_id = addComponent(d);	
		d->run();
		
		return tmp_id;
	}
	
	
	size_t Delilah::addComponent( DelilahComponent* component )
	{
		lock.lock();
		size_t tmp_id = id++;
		component->setId(this, tmp_id);
		components.insertInMap( tmp_id , component );
		lock.unlock();
		
		return tmp_id;
	}
	size_t Delilah::sendCommand(  std::string command )
	{
		// We do now create a component, but we will+

		lock.lock();
		
		size_t tmp_id = id++;

		Packet*           p = new Packet();
		network::Command* c = p->message.mutable_command();
		c->set_command( command );
		p->message.set_delilah_id( tmp_id );
		copyEnviroment( &environment , c->mutable_environment() );
		network->send(this, network->controllerGetIdentifier(), Message::Command, p);
		
		lock.unlock();
		
		return tmp_id;
	}	
		
	
}

