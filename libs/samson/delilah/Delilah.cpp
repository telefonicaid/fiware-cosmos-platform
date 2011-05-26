#include <iostream>				// std::cout ...

#include "logMsg/logMsg.h"             // lmInit, LM_*

#include "samson/common/Macros.h"             // EXIT, ...
#include "samson/network/Message.h"            // Message::MessageCode, ...
#include "samson/network/Packet.h"				// samson::Packet
#include "samson/network/Network.h"			// NetworkInterface
#include "samson/network/Endpoint.h"			// Endpoint
#include "au/CommandLine.h"		// CommandLine
#include "samson/delilah/Delilah.h"			// Own interfce
#include "samson/network/Packet.h"				// samson::Packet
#include "DelilahUploadDataProcess.h"	// samson::DelilahLoadDataProcess
#include "DelilahDownloadDataProcess.h"	// samson::DelilahLoadDataProcess
#include "samson/common/EnvironmentOperations.h"
#include "PushComponent.h"          // samson::PushComponent
#include "au/Cronometer.h"      // au::Cronometer

#include "PushComponent.h"   // PushDataComponent

#define notification_delilah_automatic_update "notification_delilah_automatic_update"


namespace samson {
    
	au::Lock info_lock;
	network::OperationList *ol = NULL;              // List of operations ( for auto-completion )
	network::QueueList *ql = NULL;                  // List of queues ( for auto-completion )
    network::SamsonStatus *samsonStatus=NULL;       // Information about workers ( updated continuously )
    au::Cronometer cronometer_samsonStatus;      // Cronometer for this updated message
    
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
		
        id = 2;	// we start with process 2 because 0 is no process & 1 is global_update messages
		
        finish = false;				// Global flag to finish threads
        
        // By default, no traces
        trace_on =  false;
        
        // Default component to update local list of queues and operations
        engine::Engine::add( notification_delilah_automatic_update , this );
        
        // Emit a periodic notification
        engine::Engine::add( new engine::Notification( notification_delilah_automatic_update ) , 2 );

    }
    
    
    Delilah::~Delilah()
    {
        clearAllComponents();
    }

    
    void Delilah::notify( engine::Notification* notification )
    {
        if( !notification->isName(notification_delilah_automatic_update) )
            LM_X(1,("Delilah received an unexpected notification"));
        
        
        // Send a message to the controller to receive an update of the information
        if( network->isConnected( network->controllerGetIdentifier()  ) )
        {
            
            {
				// Message to update the local list of queues
				Packet*           p = new Packet(Message::Command);
				network::Command* c = p->message->mutable_command();
				c->set_command( "ls -global_update" );
				p->message->set_delilah_id( 1 );    // Spetial id for global update
				//copyEnviroment( &environment , c->mutable_environment() );
				network->send(this, network->controllerGetIdentifier(), p);
            }
            
            {
				// Message to update the local list of operations
				Packet*           p = new Packet(Message::Command);
				network::Command* c = p->message->mutable_command();
				c->set_command( "o -global_update" );
				p->message->set_delilah_id( 1 );    // Spetial id for global update
				//copyEnviroment( &environment , c->mutable_environment() );
				network->send(this, network->controllerGetIdentifier(), p);
            }	
            
            {
                // Message to update the worker status list
                Packet*           p = new Packet(Message::Command);
                network::Command* c = p->message->mutable_command();
                c->set_command( "w -global_update" );
                p->message->set_delilah_id( 1 );    // Spetial id for global update
                //copyEnviroment( &environment , c->mutable_environment() );
                network->send(this, network->controllerGetIdentifier(), p);
            }	
            
        }
        
        
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

        // spetial case for global_update messages
        
        if( (msgCode == Message::CommandResponse) && (packet->message->delilah_id() == 1 ))
        {
            // Global update messages
            _receive_global_update( packet );
            return;
        }
        
        if( !component )
        {
            
            // Forward the reception of this message to the client
            _receive( fromId , msgCode , packet );
        }
        
    }
	

    void Delilah::_receive_global_update( Packet *packet )
    {
		// Process to update the local list of queues and operations
		std::ostringstream  txt;
		
        if( packet->message->command_response().has_queue_list() )
        {
            // Copy the list of queues for auto-completion
            info_lock.lock();
            
            if( ql )
                delete ql;
            ql = new network::QueueList();
            ql->CopyFrom( packet->message->command_response().queue_list() );
            
            info_lock.unlock();
            
        }
        
        if( packet->message->command_response().has_operation_list() )
        {
            info_lock.lock();
            
            if( ol )
                delete ol;
            ol = new network::OperationList();
            ol->CopyFrom( packet->message->command_response().operation_list() );
            
            info_lock.unlock();
        }
        
        // Update of the samson status
        if( packet->message->command_response().has_samson_status() )
        {
            // Reset the cronometer of the samsonStatus report
            cronometer_samsonStatus.reset();
            
            info_lock.lock();
            
            if( samsonStatus )
                delete samsonStatus;
            samsonStatus = new network::SamsonStatus();
            samsonStatus->CopyFrom( packet->message->command_response().samson_status() );
            
            info_lock.unlock();
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
    
	/* ****************************************************************************
     *
     * pushData - 
     */
	size_t Delilah::addPushData( std::vector<std::string> fileNames , std::string queue )
	{
		PushComponent * d = new PushComponent( fileNames , queue );
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

