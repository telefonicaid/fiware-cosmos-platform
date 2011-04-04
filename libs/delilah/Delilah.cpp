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

	au::Lock list_lock;
	network::OperationList *ol = NULL;		// List of operations ( for auto-completion )
	network::QueueList *ql = NULL;			// List of queues ( for auto-completion )
  
    network::ControllerStatus *cs=NULL;     // Controller status ( updated continously )
    network::WorkerStatusList *wl=NULL;     // Information about workers ( updated continuously )
	
	

/* ****************************************************************************
*
* Delilah::Delilah
*/
Delilah::Delilah( NetworkInterface* _network , bool automatic_update )
{
		
	// Description for the PacketReceiver
	packetReceiverDescription = "delilah";
	
	
	network = _network;		// Keep a pointer to our network interface element
	network->setPacketReceiver(this);
		
	id = 1;	// we start with process 1 because 0 is no process
		
	finish = false;				// Global flag to finish threads

	
	// Default component to update local list of queues and operations
	if( automatic_update) 
		addComponent( new DelilahUpdater( ) );
	
}
		
    
    Delilah::~Delilah()
    {
        clearAllComponents();
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
void Delilah::receive( Packet* packet )
{
	int fromId = packet->fromId;
	Message::MessageCode msgCode = packet->msgCode;
	
	token.retain();
	
	size_t sender_id = packet->message->delilah_id();
	DelilahComponent *component = components.findInMap( sender_id );

	
	if ( component )
		component->receive( fromId, msgCode, packet );
	
	token.release();
	
	if( !component )
	{
		// Forward the reception of this message to the client
		_receive( fromId , msgCode , packet );
	}


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
	size_t Delilah::addUploadData( std::vector<std::string> fileNames , std::string queue , bool compresion ,int max_num_threads)
	{
		DelilahUploadDataProcess * d = new DelilahUploadDataProcess( fileNames , queue , compresion, max_num_threads );
		
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
		token.retain();

		size_t tmp_id = id++;
		component->setId(this, tmp_id);
		components.insertInMap( tmp_id , component );

		token.release();
		
		return tmp_id;
	}
	
	void Delilah::clearComponents()
	{

		std::vector<size_t> components_to_remove;
		
		token.retain();
		
		for ( au::map<size_t , DelilahComponent>::iterator c =  components.begin() ;  c != components.end() ; c++)
			if ( c->second->component_finished )
				components_to_remove.push_back( c->first );

		for (size_t i = 0 ; i < components_to_remove.size() ; i++)
		{
			DelilahComponent *component = components.extractFromMap( components_to_remove[i] );
			if( component )
				delete component;
		}
			 
		token.release();
	}

	void Delilah::clearAllComponents()
	{
        
		std::vector<size_t> components_to_remove;
		
		token.retain();
		
		for ( au::map<size_t , DelilahComponent>::iterator c =  components.begin() ;  c != components.end() ; c++)
				components_to_remove.push_back( c->first );
        
		for (size_t i = 0 ; i < components_to_remove.size() ; i++)
		{
			DelilahComponent *component = components.extractFromMap( components_to_remove[i] );
			if( component )
				delete component;
		}
        
		token.release();
	}
	
    

	std::string Delilah::getListOfLoads()
	{
		std::stringstream output;
		bool present = false;
		output << "-----------------------------------------------------------------\n";
		output << "Upload and download processes....\n";
		output << "-----------------------------------------------------------------\n";
		output << "\n";
		std::map<size_t,DelilahComponent*>::iterator iter;
		for (iter = components.begin() ; iter != components.end() ; iter++)
		{
			if ( iter->second->type == DelilahComponent::load )
			{
				output << iter->second->getStatus() << "\n";
				present = true;
			}
		}
		
		if( !present )
			output << "\tNo upload or download process.\n";
		output << "\n";
		output << "-----------------------------------------------------------------\n";
		
		return output.str();
	}
	
    std::string Delilah::getListOfComponents()
    {
		std::stringstream output;
		bool present = false;
		output << "-----------------------------------------------------------------\n";
		output << "List of delilah processes....\n";
		output << "-----------------------------------------------------------------\n";
		output << "\n";
		std::map<size_t,DelilahComponent*>::iterator iter;
		for (iter = components.begin() ; iter != components.end() ; iter++)
		{
            output << iter->second->getStatus() << "\n";
            present = true;
		}
		
		if( !present )
			output << "\tNo processes here.\n";
		output << "\n";
		output << "-----------------------------------------------------------------\n";
		
		return output.str();    
    }
    
	size_t Delilah::sendCommand(  std::string command )
	{
		
		// Add a components for the reception
		CommandDelilahComponent *c = new CommandDelilahComponent( command );

		// Get the id of this operation
		size_t tmp_id = addComponent( c );
		
		// Send the packet to the controller
		c->run();
		
		return tmp_id;
	}	

	
	bool Delilah::isActive( size_t id )
	{
		bool ans = false;
		
		token.retain();
		
		DelilahComponent *c = components.findInMap( id );
		if( c && !c->component_finished )
			ans = true;
		
		token.release();
		
		return ans;
	}

		
	
}

