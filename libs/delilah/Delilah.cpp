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
#include "DelilahUploadDataProcess.h"	// ss::DelilahLoadDataProcess
#include "DelilahDownloadDataProcess.h"	// ss::DelilahLoadDataProcess
#include "EnvironmentOperations.h"


namespace ss {


/* ****************************************************************************
*
* Delilah::Delilah
*/
Delilah::Delilah( NetworkInterface* _network )
{
		
	network = _network;		// Keep a pointer to our network interface element
	network->setPacketReceiverInterface(this);
		
	id = 1;	// we start with process 1 because 0 is no process
		
	finish = false;				// Global flag to finish threads
		
}
		

/* ****************************************************************************
*
* quit - 
*/
void Delilah::quit()
{
	finish = true;
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
	
	
	size_t Delilah::addDownloadProcess( std::string queue , std::string fileName , bool show_on_screen )
	{
		DelilahDownloadDataProcess *d = new DelilahDownloadDataProcess( queue , fileName , show_on_screen );
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
		size_t tmp_id;
		
		lock.lock();
		tmp_id = id++;
		lock.unlock();
		
		// Send the packet to create a job
		Packet*           p = new Packet();
		network::Command* c = p->message.mutable_command();
		c->set_command( command );
		p->message.set_delilah_id( tmp_id );
		copyEnviroment( &environment , c->mutable_environment() );
		network->send(this, network->controllerGetIdentifier(), Message::Command, p);
		
		return tmp_id;
	}	
		
	
}

